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
     * block to run.
     */
    zvalue function;
} Closure;


/*
 * Helper functions
 */

/* Defined below. */
static zvalue execExpression(zcontext ctx, zvalue expression);
static zvalue execExpressionVoidOk(zcontext ctx, zvalue expression);

/**
 * Executes a `block` form.
 */
static zvalue execBlock(zcontext ctx, zvalue block) {
    datHighletAssertType(block, STR_BLOCK);
    block = datHighletValue(block);

    zvalue statements = datMapletGet(block, STR_STATEMENTS);
    zint size = datSize(statements);

    for (zint i = 0; i < size; i++) {
        zvalue one = datListletGet(statements, i);
        zvalue type = datHighletType(one);

        if (datHighletHasType(one, STR_VAR_DEF)) {
            zvalue nameValue = datHighletValue(one);
            zvalue name = datMapletGet(nameValue, STR_NAME);
            zvalue value = datMapletGet(nameValue, STR_VALUE);
            ctxBind(ctx, name, execExpression(ctx, value));
        } else {
            execExpressionVoidOk(ctx, one);
        }
    }

    zvalue yield = datMapletGet(block, STR_YIELD);
    return (yield == NULL) ? NULL : execExpressionVoidOk(ctx, yield);
}

/**
 * The C function that is used for all registrations with function
 * registries.
 */
zvalue execClosure(void *state, zint argCount, const zvalue *args) {
    Closure *closure = state;
    zvalue formals = datMapletGet(closure->function, STR_FORMALS);
    zvalue block = datMapletGet(closure->function, STR_BLOCK);

    datHighletAssertType(formals, STR_FORMALS);
    formals = datHighletValue(formals);

    zint formalsSize = datSize(formals);
    zvalue locals = datMapletEmpty();

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = datListletGet(formals, i);
        zvalue name = datMapletGet(formal, STR_NAME);
        zvalue repeat = datMapletGet(formal, STR_REPEAT);
        zvalue value;

        if (datOrder(repeat, TOK_CH_STAR) == 0) {
            value = datListletEmpty();
            for (/*i*/; i < argCount; i++) {
                value = datListletAppend(value, args[i]);
            }
        } else if (i >= argCount) {
            die("Too few arguments to function: %lld", argCount);
        } else {
            value = args[i];
        }

        locals = datMapletPut(locals, name, value);
    }

    zcontext ctx = ctxNewChild(closure->parent, locals);
    return execBlock(ctx, block);
}

/**
 * Executes a `function` form.
 */
static zvalue execFunction(zcontext ctx, zvalue function) {
    datHighletAssertType(function, STR_FUNCTION);

    Closure *closure = zalloc(sizeof(Closure));
    closure->parent = ctx;
    closure->function = datHighletValue(function);

    return funDefine(execClosure, closure);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(zcontext ctx, zvalue call) {
    datHighletAssertType(call, STR_CALL);
    call = datHighletValue(call);

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
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(zcontext ctx, zvalue e) {
    if (datHighletHasType(e, STR_LITERAL))
        return datHighletValue(e);
    else if (datHighletHasType(e, STR_VAR_REF))
        return ctxGet(ctx, datHighletValue(e));
    else if (datHighletHasType(e, STR_CALL))
        return execCall(ctx, e);
    else if (datHighletHasType(e, STR_FUNCTION))
        return execFunction(ctx, e);
    else {
        die("Invalid expression type.");
    }
}

/**
 * Executes an `expression` form, with the result never allowed to be
 * `void`.
 */
static zvalue execExpression(zcontext ctx, zvalue expression) {
    zvalue result = execExpressionVoidOk(ctx, expression);

    if (result == NULL) {
        die("Invalid use of void expression result.");
    }

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue langFunctionFromNode(zcontext ctx, zvalue functionNode) {
    return execFunction(ctx, functionNode);
}
