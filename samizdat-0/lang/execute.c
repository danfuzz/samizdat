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
} Closure;


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
    zvalue formals = datMapletGet(closure->function, STR_FORMALS);
    zvalue statements = datMapletGet(closure->function, STR_STATEMENTS);

    hidAssertType(formals, STR_FORMALS);
    formals = hidValue(formals);

    zint formalsSize = datSize(formals);
    zvalue locals = datMapletEmpty();

    if (argCount < formalsSize) {
        die("Too few arguments to function: %lld < %lld",
            argCount, formalsSize);
    }

    for (zint i = 0; i < formalsSize; i++) {
        zvalue name = datListletGet(formals, i);
        locals = datMapletPut(locals, name, args[i]);
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
    hidAssertType(function, STR_FUNCTION);

    Closure *closure = zalloc(sizeof(Closure));
    closure->parent = ctx;
    closure->function = hidValue(function);

    return funDefine(execClosure, closure);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(zcontext ctx, zvalue call) {
    hidAssertType(call, STR_CALL);
    call = hidValue(call);

    zvalue function = datMapletGet(call, STR_FUNCTION);
    zvalue actuals = datMapletGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(ctx, function);

    zint argCount = datSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = datListletGet(actuals, i);
        args[i] = execExpression(ctx, one);
    }

    return langCall(functionId, argCount, args);
}

/**
 * Executes a `maplet` form.
 */
static zvalue execMaplet(zcontext ctx, zvalue maplet) {
    hidAssertType(maplet, STR_MAPLET);

    zvalue elems = hidValue(maplet);
    zint size = datSize(elems);
    zvalue result = datMapletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = datListletGet(elems, i);
        zvalue key = datMapletGet(one, STR_KEY);
        zvalue value = datMapletGet(one, STR_VALUE);
        result = datMapletPut(result,
                              execExpression(ctx, key),
                              execExpression(ctx, value));
    }

    return result;
}

/**
 * Executes a `listlet` form.
 */
static zvalue execListlet(zcontext ctx, zvalue listlet) {
    hidAssertType(listlet, STR_LISTLET);

    zvalue elems = hidValue(listlet);
    zint size = datSize(elems);
    zvalue result = datListletEmpty();

    for (zint i = 0; i < size; i++) {
        zvalue one = datListletGet(elems, i);
        result = datListletAppend(result, execExpression(ctx, one));
    }

    return result;
}

/**
 * Executes an `expression` form.
 */
static zvalue execExpression(zcontext ctx, zvalue e) {
    if      (hidHasType(e, STR_LITERAL))  { return hidValue(e);              }
    else if (hidHasType(e, STR_VAR_REF))  { return ctxGet(ctx, hidValue(e)); }
    else if (hidHasType(e, STR_LISTLET))  { return execListlet(ctx, e);      }
    else if (hidHasType(e, STR_MAPLET))   { return execMaplet(ctx, e);       }
    else if (hidHasType(e, STR_UNIQLET))  { return datUniqlet();             }
    else if (hidHasType(e, STR_CALL))     { return execCall(ctx, e);         }
    else if (hidHasType(e, STR_FUNCTION)) { return execFunction(ctx, e);     }
    else {
        die("Invalid expression type.");
    }
}

/**
 * Executes a `statements` form.
 */
static void execStatements(zcontext ctx, zvalue statements) {
    hidAssertType(statements, STR_STATEMENTS);
    statements = hidValue(statements);

    zint size = datSize(statements);
    for (zint i = 0; (i < size) && (ctx->toReturn == NULL); i++) {
        zvalue one = datListletGet(statements, i);
        zvalue type = hidType(one);

        if (hidHasType(one, STR_VAR_DEF)) {
            zvalue nameValue = hidValue(one);
            zvalue name = datMapletGet(nameValue, STR_NAME);
            zvalue value = datMapletGet(nameValue, STR_VALUE);

            ctxBind(ctx, name, execExpression(ctx, value));
        } else if (hidHasType(one, STR_RETURN)) {
            ctx->toReturn = execExpression(ctx, hidValue(one));
        } else {
            execExpression(ctx, one);
        }
    }
}


/*
 * Module functions
 */

/* Documented in header. */
void langExecute(zcontext ctx, zvalue code) {
    execStatements(ctx, code);
}
