/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
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
 * Binds variables for all the formal arguments of the given
 * function (if any), returning a maplet of the bindings.
 */
static zvalue bindArguments(zvalue functionNode,
                            zint argCount, const zvalue *args) {
    zvalue result = datMapletEmpty();
    zvalue formals = datMapletGet(functionNode, STR_FORMALS);

    if (formals == NULL) {
        return result;
    }

    datHighletAssertType(formals, STR_FORMALS);
    formals = datHighletValue(formals);

    zint formalsSize = datSize(formals);

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = datListletNth(formals, i);
        zvalue name = datMapletGet(formal, STR_NAME);
        zvalue repeat = datMapletGet(formal, STR_REPEAT);
        zvalue value;

        if (repeat != NULL) {
            if (datOrder(repeat, TOK_CH_STAR) == 0) {
                value = datListletEmpty();
                for (/*i*/; i < argCount; i++) {
                    value = datListletAppend(value, args[i]);
                }
            } else {
                die("Unknown repeat modifier.");
            }
        } else if (i >= argCount) {
            die("Too few arguments to function: %lld", argCount);
        } else {
            value = args[i];
        }

        result = datMapletPut(result, name, value);
    }

    return result;
}

/**
 * The C function that is used for all registrations with function
 * registries.
 */
static zvalue execClosure(void *state, zint argCount, const zvalue *args) {
    Closure *closure = state;
    zvalue functionNode = closure->function;
    zvalue statements = datMapletGet(functionNode, STR_STATEMENTS);
    zvalue yield = datMapletGet(functionNode, STR_YIELD);

    // Bind the formals, creating a context.
    zvalue locals = bindArguments(functionNode, argCount, args);
    zcontext ctx = ctxNewChild(closure->parent, locals);

    // Using the new context, evaluate the statements.

    zint statementsSize = datSize(statements);
    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = datListletNth(statements, i);

        if (datHighletTypeIs(one, STR_VAR_DEF)) {
            zvalue nameValue = datHighletValue(one);
            zvalue name = datMapletGet(nameValue, STR_NAME);
            zvalue value = datMapletGet(nameValue, STR_VALUE);
            ctxBind(ctx, name, execExpression(ctx, value));
        } else {
            execExpressionVoidOk(ctx, one);
        }
    }

    // Evaluate the yield expression if present, and return the final
    // result.

    return (yield == NULL) ? NULL : execExpressionVoidOk(ctx, yield);
}

/**
 * Executes a `function` form.
 */
static zvalue execFunction(zcontext ctx, zvalue function) {
    datHighletAssertType(function, STR_FUNCTION);

    Closure *closure = zalloc(sizeof(Closure));
    closure->parent = ctx;
    closure->function = datHighletValue(function);

    return langDefineFunction(execClosure, closure);
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
        zvalue one = datListletNth(actuals, i);
        args[i] = execExpression(ctx, one);
    }

    return langCall(functionId, argCount, args);
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(zcontext ctx, zvalue e) {
    if (datHighletTypeIs(e, STR_LITERAL))
        return datHighletValue(e);
    else if (datHighletTypeIs(e, STR_VAR_REF))
        return ctxGet(ctx, datHighletValue(e));
    else if (datHighletTypeIs(e, STR_CALL))
        return execCall(ctx, e);
    else if (datHighletTypeIs(e, STR_FUNCTION))
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
zvalue langEvalExpressionNode(zcontext ctx, zvalue node) {
    return execExpressionVoidOk(ctx, node);
}
