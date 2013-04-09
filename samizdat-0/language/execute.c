/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


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
    zcontext parent;

    /**
     * Function definition, which includes a list of formals and the
     * statements to run.
     */
    zvalue function;
}
Closure;


/*
 * Helper functions
 */

/* Defined below. */
static zvalue execExpression(zcontext ctx, zvalue expression);
static void execStatements(zcontext ctx, zvalue statements);

/**
 * The C function that is used for all registrations with function
 * registries.
 */
zvalue execClosure(void *state, zint argCount, const zvalue *args) {
    Closure *closure = state;
    zvalue formals = samMapletGet(closure->function, STR_FORMALS);
    zvalue statements = samMapletGet(closure->function, STR_STATEMENTS);
    zint formalsSize = samSize(formals);
    zvalue locals = samMapletEmpty();

    if (argCount < formalsSize) {
        die("Too few arguments to function: %lld < %lld",
            argCount, formalsSize);
    }

    for (zint i = 0; i < formalsSize; i++) {
        zvalue name = samListletGet(formals, i);
        locals = samMapletPut(locals, name, args[i]);
    }

    zcontext ctx = ctxNewChild(closure->parent, locals);
    execStatements(ctx, statements);

    zvalue result = ctx->toReturn;
    return (result == NULL) ? TOK_NULL : result;
}

/**
 * Executes a `function` form.
 */
static zvalue execFunction(zcontext ctx, zvalue function) {
    assertType(function, STR_FUNCTION);

    Closure *closure = zalloc(sizeof(Closure));
    closure->parent = ctx;
    closure->function = highValue(function);

    return funAdd(ctx->reg, execClosure, closure);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(zcontext ctx, zvalue call) {
    assertType(call, STR_CALL);

    call = highValue(call);
    zvalue function = samMapletGet(call, STR_FUNCTION);
    zvalue actuals = samMapletGet(call, STR_ACTUALS);

    zvalue id = execExpression(ctx, function);
    samAssertUniqlet(id);

    zint argCount = samSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = samListletGet(actuals, i);
        args[i] = execExpression(ctx, one);
    }

    return funCall(ctx->reg, id, argCount, args);
}

/**
 * Executes a `uniqlet` form.
 */
static zvalue execUniqlet(zcontext ctx, zvalue uniqlet) {
    assertType(uniqlet, STR_UNIQLET);

    return samUniqlet();
}

/**
 * Executes a `maplet` form.
 */
static zvalue execMaplet(zcontext ctx, zvalue maplet) {
    assertType(maplet, STR_MAPLET);

    zvalue elems = highValue(maplet);
    zint size = samSize(elems);
    zvalue result = samListletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(elems, i);
        zvalue key = samMapletGet(one, STR_KEY);
        zvalue value = samMapletGet(one, STR_VALUE);
        result = samMapletPut(result,
                              execExpression(ctx, key),
                              execExpression(ctx, value));
    }

    return result;
}

/**
 * Executes a `listlet` form.
 */
static zvalue execListlet(zcontext ctx, zvalue listlet) {
    assertType(listlet, STR_LISTLET);

    zvalue elems = highValue(listlet);
    zint size = samSize(elems);
    zvalue result = samListletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(elems, i);
        result = samListletAppend(result, execExpression(ctx, one));
    }

    return result;
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(zcontext ctx, zvalue varRef) {
    assertType(varRef, STR_VAR_REF);

    zvalue name = highValue(varRef);

    for (/* ctx */; ctx != NULL; ctx = ctx->parent) {
        zvalue found = samMapletGet(ctx->locals, name);
        if (found != NULL) {
            return found;
        }
    }

    die("No such variable.");
}

/**
 * Executes a `literal` form.
 */
static zvalue execLiteral(zcontext ctx, zvalue literal) {
    assertType(literal, STR_LITERAL);

    return highValue(literal);
}

/**
 * Executes an `expression` form.
 */
static zvalue execExpression(zcontext ctx, zvalue ex) {
    if      (hasType(ex, STR_LITERAL))  { return execLiteral(ctx, ex);  }
    else if (hasType(ex, STR_VAR_REF))  { return execVarRef(ctx, ex);   }
    else if (hasType(ex, STR_LISTLET))  { return execListlet(ctx, ex);  }
    else if (hasType(ex, STR_MAPLET))   { return execMaplet(ctx, ex);   }
    else if (hasType(ex, STR_UNIQLET))  { return execUniqlet(ctx, ex);  }
    else if (hasType(ex, STR_CALL))     { return execCall(ctx, ex);     }
    else if (hasType(ex, STR_FUNCTION)) { return execFunction(ctx, ex); }
    else {
        die("Invalid expression type.");
    }
}

/**
 * Executes a `varDef` form.
 */
static void execVarDef(zcontext ctx, zvalue varDef) {
    assertType(varDef, STR_VAR_DEF);

    zvalue nameValue = highValue(varDef);
    zvalue name = samMapletGet(nameValue, STR_NAME);
    zvalue value = samMapletGet(nameValue, STR_VALUE);

    if (samMapletGet(ctx->locals, name) != NULL) {
        die("Duplicate assignment.");
    }

    ctx->locals = samMapletPut(ctx->locals, name, execExpression(ctx, value));
}

/**
 * Executes a `return` form.
 */
static void execReturn(zcontext ctx, zvalue returnForm) {
    assertType(returnForm, STR_RETURN);

    ctx->toReturn = execExpression(ctx, highValue(returnForm));
}

/**
 * Executes a `statements` form.
 */
static void execStatements(zcontext ctx, zvalue statements) {
    assertType(statements, STR_STATEMENTS);

    statements = highValue(statements);
    zint size = samSize(statements);

    for (zint i = 0; (i < size) && (ctx->toReturn == NULL); i++) {
        zvalue one = samListletGet(statements, i);
        zvalue type = highType(one);
        if (hasType(one, STR_EXPRESSION)) {
            execExpression(ctx, one);
        } else if (hasType(one, STR_VAR_DEF)) {
            execVarDef(ctx, one);
        } else if (hasType(one, STR_RETURN)) {
            execReturn(ctx, one);
        } else {
            die("Invalid statements element.");
        }
    }
}


/*
 * Module functions
 */

/* Documented in header. */
void samExecute(zcontext ctx, zvalue code) {
    execStatements(ctx, code);
}
