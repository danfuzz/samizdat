/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <setjmp.h>
#include <stddef.h>


/**
 * Closure, that is, a function and its associated immutable bindings.
 * Instances of this structure are bound as the closure state as part of
 * function registration in `execFunction()`.
 */
typedef struct {
    /**
     * Parent variable context (which was the current context when
     * the function was defined).
     */
    zvalue context;

    /**
     * Function definition, which includes a list of formals and the
     * block to run.
     */
    zvalue function;
} Closure;

/**
 * Nonlocal exit pending state. Instances of this structure are bound as
 * the closure state as part of call setup for closures that have a
 * `yieldDef`.
 */
typedef struct {
    /**
     * Whether this state is active, in that there is a valid nonlocal exit
     * that could land here.
     */
    bool active;

    /** Return value thrown here via nonlocal exit. `NULL` for void. */
    zvalue result;

    /** Jump buffer, used for nonlocal exit. */
    jmp_buf jumpBuf;
} YieldState;


/*
 * Helper functions
 */

/* Defined below. */
static zvalue execExpression(zvalue ctx, zvalue expression);
static zvalue execExpressionVoidOk(zvalue ctx, zvalue expression);

/**
 * The C function that is bound to in order to perform nonlocal exit.
 */
static zvalue nonlocalExit(void *state, zint argCount, const zvalue *args) {
    YieldState *yield = state;

    if (yield->active) {
        yield->active = false;
    } else {
        die("Attempt to use out-of-scope nonlocal exit.");
    }

    switch (argCount) {
        case 0: {
            // Nothing to do.
            break;
        }
        case 1: {
            yield->result = args[0];
            break;
        }
        default: {
            die("Too many arguments for nonlocal exit: %lld", argCount);
        }
    }

    longjmp(yield->jumpBuf, 1);
}

/**
 * Binds variables for all the formal arguments of the given
 * function (if any), returning a maplet of the bindings added
 * to the given base context.
 */
static zvalue bindArguments(zvalue ctx, zvalue functionNode,
                            zint argCount, const zvalue *args) {
    zvalue formals = datMapletGet(functionNode, STR_FORMALS);

    if (formals == NULL) {
        return ctx;
    }

    zint formalsSize = datSize(formals);
    zmapping mappings[formalsSize];

    for (zint i = 0, argAt = 0; i < formalsSize; i++) {
        zvalue formal = datListletNth(formals, i);
        zvalue name = datMapletGet(formal, STR_NAME);
        zvalue repeat = datMapletGet(formal, STR_REPEAT);
        zvalue value;

        if (repeat != NULL) {
            if (datOrder(repeat, TOK_CH_STAR) == 0) {
                value = datListletFromArray(argCount - argAt, &args[argAt]);
                argAt = argCount;
            } else if (datOrder(repeat, TOK_CH_QMARK) == 0) {
                if (argAt < argCount) {
                    value = datListletFromArray(1, &args[argAt]);
                    argAt++;
                } else {
                    value = EMPTY_LISTLET;
                }
            } else {
                die("Unknown repeat modifier.");
            }
        } else if (i >= argCount) {
            die("Too few arguments to function: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        mappings[i].key = name;
        mappings[i].value = value;
    }

    return datMapletAddArray(ctx, formalsSize, mappings);
}

/**
 * Executes a variable definition, by updating the given variable
 * context as appropriate.
 */
static zvalue execVarDef(zvalue ctx, zvalue varDef) {
    zvalue nameValue = datHighletValue(varDef);
    zvalue name = datMapletGet(nameValue, STR_NAME);
    zvalue valueExpression = datMapletGet(nameValue, STR_VALUE);
    zvalue value = execExpression(ctx, valueExpression);

    return datMapletPut(ctx, name, value);
}

/**
 * The C function that is bound to in order to execute interpreted code.
 */
static zvalue execClosure(void *state, zint argCount, const zvalue *args) {
    Closure *closure = state;
    zvalue functionNode = closure->function;
    zvalue parentContext = closure->context;

    zvalue yieldDef = datMapletGet(functionNode, STR_YIELD_DEF);
    zvalue statements = datMapletGet(functionNode, STR_STATEMENTS);
    zvalue yield = datMapletGet(functionNode, STR_YIELD);
    YieldState *yieldState = NULL;

    // Take the parent context as a base, and bind the formals and
    // yieldDef (if present), creating an initial variable context.

    zvalue ctx = bindArguments(parentContext, functionNode, argCount, args);

    if (yieldDef != NULL) {
        yieldState = zalloc(sizeof(YieldState));
        yieldState->active = true;
        yieldState->result = NULL;

        if (setjmp(yieldState->jumpBuf) != 0) {
            // Here is where we land if and when `longjmp` is called.
            return yieldState->result;
        }

        zvalue exitFunction = langDefineFunction(nonlocalExit, yieldState);
        ctx = datMapletPut(ctx, yieldDef, exitFunction);
    }

    // Evaluate the statements, updating the variable context as needed.

    zint statementsSize = datSize(statements);
    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = datListletNth(statements, i);

        if (datHighletTypeIs(one, STR_VAR_DEF)) {
            ctx = execVarDef(ctx, one);
        } else {
            execExpressionVoidOk(ctx, one);
        }
    }

    // Evaluate the yield expression if present, and return the final
    // result.

    zvalue result = NULL;

    if (yield != NULL) {
        result = execExpressionVoidOk(ctx, yield);
    }

    if (yieldState != NULL) {
        yieldState->active = false;
    }

    return result;
}

/**
 * Executes a `function` form.
 */
static zvalue execFunction(zvalue ctx, zvalue function) {
    datHighletAssertType(function, STR_FUNCTION);

    Closure *closure = zalloc(sizeof(Closure));
    closure->context = ctx;
    closure->function = datHighletValue(function);

    return langDefineFunction(execClosure, closure);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(zvalue ctx, zvalue call) {
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
 * Executes a `varRef` form.
 */
static zvalue execVarRef(zvalue ctx, zvalue varRef) {
    datHighletAssertType(varRef, STR_VAR_REF);

    zvalue name = datHighletValue(varRef);
    zvalue found = datMapletGet(ctx, name);

    if (found != NULL) {
        return found;
    }

    if (datTypeIs(name, DAT_STRINGLET)) {
        zint nameSize = datUtf8SizeFromStringlet(name);
        char nameStr[nameSize + 1];
        datUtf8FromStringlet(nameSize + 1, nameStr, name);
        die("No such variable: %s", nameStr);
    }

    die("No such variable: (strange name)");
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(zvalue ctx, zvalue e) {
    if (datHighletTypeIs(e, STR_LITERAL))
        return datHighletValue(e);
    else if (datHighletTypeIs(e, STR_VAR_REF))
        return execVarRef(ctx, e);
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
static zvalue execExpression(zvalue ctx, zvalue expression) {
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
zvalue langEvalExpressionNode(zvalue ctx, zvalue node) {
    return execExpressionVoidOk(ctx, node);
}
