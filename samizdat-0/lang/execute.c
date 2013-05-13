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


/*
 * Helper definitions
 */

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

/**
 * Marks a closure state for garbage collection.
 */
static void closureMark(void *state) {
    Closure *closure = state;

    datMark(closure->context);
    datMark(closure->function);
}

/**
 * Frees a closure state.
 */
static void closureFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for closures. */
static DatUniqletDispatch CLOSURE_DISPATCH = {
    closureMark,
    closureFree
};

/**
 * Marks a yield state for garbage collection.
 */
static void yieldMark(void *state) {
    YieldState *yield = state;

    datMark(yield->result);
}

/**
 * Frees a yield state.
 */
static void yieldFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for yield states. */
static DatUniqletDispatch YIELD_DISPATCH = {
    yieldMark,
    yieldFree
};


/* Defined below. */
static zvalue execExpression(zvalue ctx, zvalue expression);
static zvalue execExpressionVoidOk(zvalue ctx, zvalue expression);

/**
 * The C function that is bound to in order to perform nonlocal exit.
 */
static zvalue nonlocalExit(zvalue state, zint argCount, const zvalue *args) {
    YieldState *yield = datUniqletGetState(state, &YIELD_DISPATCH);

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
 * function (if any), returning a map of the bindings added
 * to the given base context.
 */
static zvalue bindArguments(zvalue ctx, zvalue functionNode,
                            zint argCount, const zvalue *args) {
    zvalue formals = datMapGet(functionNode, STR_FORMALS);

    if (formals == NULL) {
        return ctx;
    }

    zint formalsSize = datSize(formals);
    zmapping bindings[formalsSize];
    zint bindAt = 0;

    for (zint i = 0, argAt = 0; i < formalsSize; i++) {
        zvalue formal = datListNth(formals, i);
        zvalue name = datMapGet(formal, STR_NAME);
        zvalue repeat = datMapGet(formal, STR_REPEAT);
        bool ignore = (name == NULL);
        zvalue value = NULL;

        if (repeat != NULL) {
            if (datEq(repeat, TOK_CH_STAR)) {
                if (!ignore) {
                    value = datListFromArray(argCount - argAt, &args[argAt]);
                }
                argAt = argCount;
            } else if (datEq(repeat, TOK_CH_QMARK)) {
                if (argAt < argCount) {
                    if (!ignore) {
                        value = datListFromArray(1, &args[argAt]);
                    }
                    argAt++;
                } else {
                    value = EMPTY_LIST;
                }
            } else {
                die("Unknown repeat modifier.");
            }
        } else if (argAt >= argCount) {
            die("Too few arguments to function: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        if (!ignore) {
            bindings[bindAt].key = name;
            bindings[bindAt].value = value;
            bindAt++;
        }
    }

    return datMapAddArray(ctx, bindAt, bindings);
}

/**
 * Executes a variable definition, by updating the given variable
 * context as appropriate.
 */
static zvalue execVarDef(zvalue ctx, zvalue varDef) {
    zvalue nameValue = datHighletValue(varDef);
    zvalue name = datMapGet(nameValue, STR_NAME);
    zvalue valueExpression = datMapGet(nameValue, STR_VALUE);
    zvalue value = execExpression(ctx, valueExpression);

    return datMapPut(ctx, name, value);
}

/**
 * The C function that is bound to in order to execute interpreted code.
 */
static zvalue execClosure(zvalue state, zint argCount, const zvalue *args) {
    Closure *closure = datUniqletGetState(state, &CLOSURE_DISPATCH);
    zvalue functionNode = closure->function;
    zvalue parentContext = closure->context;

    zvalue yieldDef = datMapGet(functionNode, STR_YIELD_DEF);
    zvalue statements = datMapGet(functionNode, STR_STATEMENTS);
    zvalue yield = datMapGet(functionNode, STR_YIELD);
    YieldState *yieldState = NULL;

    // Take the parent context as a base, and bind the formals and
    // yieldDef (if present), creating an initial variable context.

    zvalue ctx = bindArguments(parentContext, functionNode, argCount, args);

    if (yieldDef != NULL) {
        yieldState = utilAlloc(sizeof(YieldState));
        yieldState->active = true;
        yieldState->result = NULL;

        if (setjmp(yieldState->jumpBuf) != 0) {
            // Here is where we land if and when `longjmp` is called.
            return yieldState->result;
        }

        zvalue exitFunction =
            langDefineFunction(nonlocalExit,
                               datUniqletWith(&YIELD_DISPATCH, yieldState));
        ctx = datMapPut(ctx, yieldDef, exitFunction);
    }

    // Evaluate the statements, updating the variable context as needed.

    zint statementsSize = datSize(statements);
    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = datListNth(statements, i);

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

    Closure *closure = utilAlloc(sizeof(Closure));
    closure->context = ctx;
    closure->function = datHighletValue(function);

    return langDefineFunction(execClosure,
                              datUniqletWith(&CLOSURE_DISPATCH, closure));
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(zvalue ctx, zvalue call) {
    datHighletAssertType(call, STR_CALL);
    call = datHighletValue(call);

    zvalue function = datMapGet(call, STR_FUNCTION);
    zvalue actuals = datMapGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(ctx, function);

    zint argCount = datSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = datListNth(actuals, i);
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
    zvalue found = datMapGet(ctx, name);

    if (found != NULL) {
        return found;
    }

    if (datTypeIs(name, DAT_STRING)) {
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
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
