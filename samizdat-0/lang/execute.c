/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Closures
 */

/* Defined below. */
static zvalue execExpression(Frame *frame, zvalue expression);
static zvalue execExpressionVoidOk(Frame *frame, zvalue expression);
static void execVarDef(Frame *frame, zvalue varDef);
static void execFnDefs(Frame *frame, zint size, const zvalue *statements);

/**
 * Closure, that is, a function and its associated immutable bindings.
 * Instances of this structure are bound as the closure state as part of
 * function registration in `execClosure()`.
 */
typedef struct {
    /**
     * Snapshot of the frame that was active at the moment the closure was
     * constructed.
     */
    Frame frame;

    /**
     * Closure payload map that represents the fixed definition of the
     * closure. This can be the payload of either a `closure` or a
     * `fnDef` node.
     */
    zvalue defMap;
} Closure;

/**
 * Marks a closure state for garbage collection.
 */
static void closureMark(void *state) {
    Closure *closure = state;

    frameMark(&closure->frame);
    pbMark(closure->defMap);
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
 * Function call state. This is used during function call setup. Pointers
 * to these are passed directly within this file as well as passed
 * indirectly via the nonlocal exit handling code.
 */
typedef struct {
    /** Closure being called (in uniqlet wrapper). */
    zvalue closureValue;

    /** Closure being called (struct pointer). */
    Closure *closure;

    /** Argument count. */
    zint argCount;

    /** Array of arguments. */
    const zvalue *args;
} CallState;

/**
 * Binds variables for all the formal arguments of the given
 * function (if any), into the given execution frame.
 */
static void bindArguments(Frame *frame, zvalue node,
                          zint argCount, const zvalue *args) {
    zvalue formals = datMapGet(node, STR_FORMALS);

    if (formals == NULL) {
        return;
    }

    zint formalsSize = datSize(formals);
    zvalue formalsArr[formalsSize];
    zint argAt = 0;

    datArrayFromList(formalsArr, formals);

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = formalsArr[i];
        zvalue name = datMapGet(formal, STR_NAME);
        zvalue repeat = datMapGet(formal, STR_REPEAT);
        bool ignore = (name == NULL);
        zvalue value;

        if (repeat != NULL) {
            zint count;

            if ((datSize(repeat) != 1) || !datCoreTypeIs(repeat, DAT_String)) {
                die("Invalid repeat modifier (non-string).");
            }

            switch (datStringNth(repeat, 0)) {
                case '*': {
                    count = argCount - argAt;
                    break;
                }
                case '+': {
                    if (argAt >= argCount) {
                        die("Function called with too few arguments "
                            "(plus argument): %lld",
                            argCount);
                    }
                    count = argCount - argAt;
                    break;
                }
                case '?': {
                    count = (argAt >= argCount) ? 0 : 1;
                    break;
                }
                default: {
                    die("Invalid repeat modifier.");
                }
            }

            if (count == 0) {
                value = EMPTY_LIST;
            } else {
                value = ignore ? NULL : datListFromArray(count, &args[argAt]);
                argAt += count;
            }
        } else if (argAt >= argCount) {
            die("Function called with too few arguments: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        if (!ignore) {
            frameAdd(frame, name, value);
        }
    }

    if (argAt != argCount) {
        die("Function called with too many arguments: %lld > %lld",
            argCount, argAt);
    }
}

/**
 * Helper that does the main work of `callClosure`, including nonlocal
 * exit binding when appropriate.
 */
static zvalue callClosureMain(CallState *callState, zvalue exitFunction) {
    zvalue closureValue = callState->closureValue;
    Closure *closure = callState->closure;
    zvalue defMap = closure->defMap;
    zint argCount = callState->argCount;
    const zvalue *args = callState->args;

    zvalue statements = datMapGet(defMap, STR_STATEMENTS);
    zvalue yield = datMapGet(defMap, STR_YIELD);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    frameInit(&frame, &closure->frame, closureValue, EMPTY_MAP);
    bindArguments(&frame, defMap, argCount, args);

    if (exitFunction != NULL) {
        zvalue name = datMapGet(defMap, STR_YIELD_DEF);
        frameAdd(&frame, name, exitFunction);
    }

    // Evaluate the statements, updating the frame as needed.

    zint statementsSize = datSize(statements);
    zvalue statementsArr[statementsSize];
    datArrayFromList(statementsArr, statements);

    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = statementsArr[i];
        zvalue oneType = datTypeOf(one);

        // Switch on size of type string to avoid gratuitous `datEq` tests.
        switch (datSize(oneType)) {
            case 5: {
                if (datEq(oneType, STR_FN_DEF)) {
                    // Look for immediately adjacent `fnDef` nodes, and
                    // process them all together.
                    zint end = i + 1;
                    for (/*end*/; end < statementsSize; end++) {
                        zvalue one = statementsArr[end];
                        if (!datTypeIs(one, STR_FN_DEF)) {
                            break;
                        }
                    }
                    execFnDefs(&frame, end - i, &statementsArr[i]);
                    i = end - 1;
                } else {
                    execExpressionVoidOk(&frame, one);
                }
                break;
            }
            case 6: {
                if (datEq(oneType, STR_VAR_DEF)) {
                    execVarDef(&frame, one);
                } else {
                    execExpressionVoidOk(&frame, one);
                }
                break;
            }
            default: {
                execExpressionVoidOk(&frame, one);
            }
        }
    }

    // Evaluate the yield expression if present, and return the final
    // result.

    return (yield == NULL) ? NULL : execExpressionVoidOk(&frame, yield);
}

/**
 * Nonlocal exit callthrough function. This is called by `nleCall`.
 */
static zvalue callClosureWithNle(void *state, zvalue exitFunction) {
    return callClosureMain((CallState *) state, exitFunction);
}

/**
 * The C function that is bound to in order to execute interpreted code.
 */
static zvalue callClosure(zvalue state, zint argCount, const zvalue *args) {
    Closure *closure = datUniqletGetState(state, &CLOSURE_DISPATCH);
    CallState callState = { state, closure, argCount, args };

    zvalue result;

    if (datMapGet(closure->defMap, STR_YIELD_DEF) != NULL) {
        result = nleCall(callClosureWithNle, &callState);
    } else {
        result = callClosureMain(&callState, NULL);
    }

    return result;
}

/**
 * Helper for evaluating `closure` and `fnDef` nodes. This does the
 * evaluation, and allows a pointer to the `Closure` struct to be
 * returned (via an out argument).
 */
static zvalue buildClosure(Closure **resultClosure, Frame *frame, zvalue node) {
    Closure *closure = utilAlloc(sizeof(Closure));
    zvalue defMap = datDataOf(node);

    frameSnap(&closure->frame, frame);
    closure->defMap = defMap;

    if (resultClosure != NULL) {
        *resultClosure = closure;
    }

    return datFnFrom(
        0, -1,
        callClosure,
        datUniqletWith(&CLOSURE_DISPATCH, closure),
        datMapGet(defMap, STR_NAME));
}


/*
 * Node evaluation
 */

/**
 * Executes a variable definition, by updating the given execution frame,
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = datDataOf(varDef);
    zvalue name = datMapGet(nameValue, STR_NAME);
    zvalue valueExpression = datMapGet(nameValue, STR_VALUE);
    zvalue value = execExpression(frame, valueExpression);

    frameAdd(frame, name, value);
}

/**
 * Executes a sequence of one or more statement-level function definitions,
 * from the `start` index (inclusive) to the `end` index (exclusive) in the
 * given `statements` list.
 */
static void execFnDefs(Frame *frame, zint size, const zvalue *statements) {
    Closure *closures[size];

    for (zint i = 0; i < size; i++) {
        zvalue one = statements[i];
        zvalue fnMap = datDataOf(one);
        zvalue name = datMapGet(fnMap, STR_NAME);
        frameAdd(frame, name, buildClosure(&closures[i], frame, one));
    }

    // Rewrite the local variable context of all the constructed closures
    // to be the *current* context. This allows for self-recursion when
    // `size == 1` and mutual recursion when `size > 1`.

    for (zint i = 0; i < size; i++) {
        frameSnap(&closures[i]->frame, frame);
    }
}

/**
 * Executes a `closure` form.
 */
static zvalue execClosure(Frame *frame, zvalue closureNode) {
    return buildClosure(NULL, frame, closureNode);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    call = datDataOf(call);

    zvalue function = datMapGet(call, STR_FUNCTION);
    zvalue actuals = datMapGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(frame, function);

    zint argCount = datSize(actuals);
    zvalue actualsArr[argCount];
    zvalue args[argCount];

    datArrayFromList(actualsArr, actuals);

    // If there are any interpolated arguments, then `interpolateAny` is
    // set to `true`, and `fullCount` indicates the count of arguments
    // after interpolation.
    zint fullCount = 0;
    bool interpolateAny = false;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = actualsArr[i];
        zvalue oneType = datTypeOf(one);
        bool voidable;
        bool interpolate;
        zvalue eval;

        if (datEq(oneType, STR_VOIDABLE)) {
            // We replace the value in `actualsArr` with the voidable
            // payload in order to keep the follow-up interpolation loop
            // simpler.
            one = actualsArr[i] = datDataOf(one);
            oneType = datTypeOf(one);
            voidable = true;
        } else {
            voidable = false;
        }

        if (datEq(oneType, STR_INTERPOLATE)) {
            one = datDataOf(one);
            interpolate = true;
        } else {
            interpolate = false;
        }

        if (voidable) {
            eval = execExpressionVoidOk(frame, one);
            if (eval == NULL) {
                return NULL;
            }
        } else {
            eval = execExpression(frame, one);
        }

        if (interpolate) {
            eval = constCollectGenerator(eval);
            args[i] = eval;
            fullCount += datSize(eval);
            interpolateAny = true;
        } else {
            args[i] = eval;
            fullCount++;
        }
    }

    if (interpolateAny) {
        zvalue fullArgs[fullCount];
        zint at = 0;

        // Build the flattened argument list.
        for (zint i = 0; i < argCount; i++) {
            zvalue oneNode = actualsArr[i];
            zvalue oneArg = args[i];
            if (datTypeIs(oneNode, STR_INTERPOLATE)) {
                datArrayFromList(&fullArgs[at], oneArg);
                at += datSize(oneArg);
            } else {
                fullArgs[at] = oneArg;
                at++;
            }
        }

        return datCall(functionId, fullCount, fullArgs);
    } else {
        return datCall(functionId, argCount, args);
    }
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = datDataOf(varRef);
    return frameGet(frame, name);
}

/**
 * Executes an `interpolate` form.
 */
static zvalue execInterpolate(Frame *frame, zvalue interpolate) {
    zvalue result = execExpressionVoidOk(frame, datDataOf(interpolate));

    if (result == NULL) {
        die("Attempt to interpolate void.");
    }

    result = constCollectGenerator(result);

    switch (datSize(result)) {
        case 0: return NULL;
        case 1: return datListNth(result, 0);
        default: {
            die("Attempt to interpolate multiple values.");
        }
    }
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    zvalue type = datTypeOf(e);

    // Switching on the size of the type is a bit of a hack. It lets us
    // avoid having to have a single big cascading `if` with a lot of
    // `datEq` calls.
    switch (datSize(type)) {
        case 4: {
            if (datEq(type, STR_CALL))
                return execCall(frame, e);
            break;
        }
        case 6: {
            if (datEq(type, STR_VAR_REF))
                return execVarRef(frame, e);
            break;
        }
        case 7: {
            if (datEq(type, STR_LITERAL))
                return datDataOf(e);
            else if (datEq(type, STR_CLOSURE))
                return execClosure(frame, e);
            break;
        }
        case 10: {
            if (datEq(type, STR_EXPRESSION))
                return execExpressionVoidOk(frame, datDataOf(e));
            break;
        }
        case 11: {
            if (datEq(type, STR_INTERPOLATE))
                return execInterpolate(frame, e);
            break;
        }
    }

    die("Invalid expression type.");
}

/**
 * Executes an `expression` form, with the result never allowed to be
 * `void`.
 */
static zvalue execExpression(Frame *frame, zvalue expression) {
    zvalue result = execExpressionVoidOk(frame, expression);

    if (result == NULL) {
        die("Invalid use of void expression result.");
    }

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue langEval0(zvalue context, zvalue node) {
    Frame frame;

    frameInit(&frame, NULL, NULL, context);
    return execExpressionVoidOk(&frame, node);
}
