/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "execute.h"
#include "util.h"

#include <stddef.h>


/**
 * Execution context.
 */
typedef struct Context {
    /** Variables bound at this level. */
    zvalue locals;

    /** Pending return value. */
    zvalue toReturn;

    /** Parent context. */
    struct Context *parent;

    /** Function registry. */
    zfunreg reg;
}
Context;

/**
 * Closure, that is a function and its associated state. Instances
 * of this structure are bound as the closure state as part of
 * function registration in `execFunction()`.
 */
typedef struct {
    /**
     * Parent context (which was the current context when the function
     * was defined).
     */
    Context *parent;

    /** Function node, which includes a list of formals and the code to run. */
    zvalue function;
}
Closure;


/*
 * Helper functions
 */

/* Defined below. */
static zvalue execExpression(Context *context, zvalue expression);

/**
 * Executes a `function` form.
 */
static zvalue execFunction(Context *context, zvalue function) {
    assertType(function, STR_FUNCTION);

    die("TODO");
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Context *context, zvalue call) {
    assertType(call, STR_CALL);

    call = highValue(call);
    zvalue function = samMapletGet(call, STR_FUNCTION);
    zvalue actuals = samMapletGet(call, STR_ACTUALS);

    zvalue id = execExpression(context, function);
    samAssertUniqlet(id);

    zint argCount = samSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = samListletGet(actuals, i);
        args[i] = execExpression(context, one);
    }

    return funCall(context->reg, id, argCount, args);
}

/**
 * Executes a `uniqlet` form.
 */
static zvalue execUniqlet(Context *context, zvalue uniqlet) {
    assertType(uniqlet, STR_UNIQLET);

    return samUniqlet();
}

/**
 * Executes a `maplet` form.
 */
static zvalue execMaplet(Context *context, zvalue maplet) {
    assertType(maplet, STR_MAPLET);

    zvalue elems = highValue(maplet);
    zint size = samSize(elems);
    zvalue result = samListletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(elems, i);
        zvalue key = samMapletGet(one, STR_KEY);
        zvalue value = samMapletGet(one, STR_VALUE);
        result = samMapletPut(result,
                              execExpression(context, key),
                              execExpression(context, value));
    }

    return result;
}

/**
 * Executes a `listlet` form.
 */
static zvalue execListlet(Context *context, zvalue listlet) {
    assertType(listlet, STR_LISTLET);

    zvalue elems = highValue(listlet);
    zint size = samSize(elems);
    zvalue result = samListletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(elems, i);
        result = samListletAppend(result, execExpression(context, one));
    }

    return result;
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Context *context, zvalue varRef) {
    assertType(varRef, STR_VAR_REF);

    zvalue name = highValue(varRef);

    for (/* context */; context != NULL; context = context->parent) {
        zvalue found = samMapletGet(context->locals, name);
        if (found != NULL) {
            return found;
        }
    }

    die("No such variable.");
}

/**
 * Executes a `literal` form.
 */
static zvalue execLiteral(Context *context, zvalue literal) {
    assertType(literal, STR_LITERAL);

    return highValue(literal);
}

/**
 * Executes an `expression` form.
 */
static zvalue execExpression(Context *context, zvalue ex) {
    zvalue type = highType(ex);

    if      (hasType(ex, STR_LITERAL))  { return execLiteral(context, ex);  }
    else if (hasType(ex, STR_VAR_REF))  { return execVarRef(context, ex);   }
    else if (hasType(ex, STR_LISTLET))  { return execListlet(context, ex);  }
    else if (hasType(ex, STR_MAPLET))   { return execMaplet(context, ex);   }
    else if (hasType(ex, STR_UNIQLET))  { return execUniqlet(context, ex);  }
    else if (hasType(ex, STR_CALL))     { return execCall(context, ex);     }
    else if (hasType(ex, STR_FUNCTION)) { return execFunction(context, ex); }
    else {
        die("Invalid expression type.");
    }
}

/**
 * Executes a `varDef` form.
 */
static void execVarDef(Context *context, zvalue varDef) {
    assertType(varDef, STR_VAR_DEF);

    zvalue nameValue = highValue(varDef);
    zvalue name = samMapletGet(nameValue, STR_NAME);
    zvalue value = samMapletGet(nameValue, STR_VALUE);

    if (samMapletGet(context->locals, name) != NULL) {
        die("Duplicate assignment.");
    }

    context->locals =
        samMapletPut(context->locals, name, execExpression(context, value));
}

/**
 * Executes a `return` form.
 */
static void execReturn(Context *context, zvalue returnForm) {
    assertType(returnForm, STR_RETURN);

    context->toReturn = execExpression(context, highValue(returnForm));
}

/**
 * Executes a `statements` form.
 */
static void execStatements(Context *context, zvalue statements) {
    assertType(statements, STR_STATEMENTS);

    statements = highValue(statements);
    zint size = samSize(statements);

    for (zint i = 0; (i < size) && (context->toReturn == NULL); i++) {
        zvalue one = samListletGet(statements, i);
        zvalue type = highType(one);
        if (hasType(one, STR_EXPRESSION)) {
            execExpression(context, one);
        } else if (hasType(one, STR_VAR_DEF)) {
            execVarDef(context, one);
        } else if (hasType(one, STR_RETURN)) {
            execReturn(context, one);
        } else {
            die("Invalid statements element.");
        }
    }
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue samExecute(zvalue environment, zfunreg funreg, zvalue code) {
    samAssertMaplet(environment);

    Context context = { environment, NULL, NULL, funreg };
    execStatements(&context, code);

    return context.locals;
}
