/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>
#include <string.h>


/*
 * Closures
 */

/* Defined below. */
static zvalue execExpression(Frame *frame, zvalue expression);
static zvalue execExpressionVoidOk(Frame *frame, zvalue expression);
static void execVarDef(Frame *frame, zvalue varDef);
static void execFnDefs(Frame *frame, zvalue statements, zint start, zint end);

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
    datMark(closure->defMap);
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
    zint argAt = 0;

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = datListNth(formals, i);
        zvalue name = datMapGet(formal, STR_NAME);
        zvalue repeat = datMapGet(formal, STR_REPEAT);
        bool ignore = (name == NULL);
        zvalue value = NULL;

        if (repeat != NULL) {
            if (datEq(repeat, STR_CH_STAR)) {
                if (argAt >= argCount) {
                    value = EMPTY_LIST;
                } else if (!ignore) {
                    value = datListFromArray(argCount - argAt, &args[argAt]);
                }
                argAt = argCount;
            } else if (datEq(repeat, STR_CH_PLUS)) {
                if (argAt >= argCount) {
                    die("Function called with too few arguments "
                        "(plus argument): %lld",
                        argCount);
                } else if (!ignore) {
                    value = datListFromArray(argCount - argAt, &args[argAt]);
                }
                argAt = argCount;
            } else if (datEq(repeat, STR_CH_QMARK)) {
                if (argAt >= argCount) {
                    value = EMPTY_LIST;
                } else {
                    if (!ignore) {
                        value = datListFromArray(1, &args[argAt]);
                    }
                    argAt++;
                }
            } else {
                die("Unknown repeat modifier.");
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
    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = datListNth(statements, i);

        if (datTokenTypeIs(one, STR_FN_DEF)) {
            // Look for immediately adjacent `fnDef` nodes, and process
            // them all together.
            zint end = i + 1;
            for (/*end*/; end < statementsSize; end++) {
                zvalue one = datListNth(statements, end);
                if (!datTokenTypeIs(one, STR_FN_DEF)) {
                    break;
                }
            }
            execFnDefs(&frame, statements, i, end);
            i = end - 1;
        } else if (datTokenTypeIs(one, STR_VAR_DEF)) {
            execVarDef(&frame, one);
        } else {
            execExpressionVoidOk(&frame, one);
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
 * Helper for `callClosure`. This is the function that handles emitting
 * a context string for a call, when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue defMap = state;
    zvalue name = datMapGet(defMap, STR_NAME);

    if (name != NULL) {
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
        return strdup(nameStr);
    } else {
        return "(unknown)";
    }
}

/**
 * The C function that is bound to in order to execute interpreted code.
 */
static zvalue callClosure(zvalue state, zint argCount, const zvalue *args) {
    Closure *closure = datUniqletGetState(state, &CLOSURE_DISPATCH);
    CallState callState = { state, closure, argCount, args };

    debugPush(callReporter, closure->defMap);

    zvalue result;

    if (datMapGet(closure->defMap, STR_YIELD_DEF) != NULL) {
        result = nleCall(callClosureWithNle, &callState);
    } else {
        result = callClosureMain(&callState, NULL);
    }

    debugPop();
    return result;
}

/**
 * Helper for evaluating `closure` and `fnDef` nodes. This does the
 * evaluation, and allows a pointer to the `Closure` struct to be
 * returned (via an out argument).
 */
static zvalue buildClosure(Closure **resultClosure, Frame *frame, zvalue node) {
    Closure *closure = utilAlloc(sizeof(Closure));

    frameSnap(&closure->frame, frame);
    closure->defMap = datTokenValue(node);

    if (resultClosure != NULL) {
        *resultClosure = closure;
    }

    return langDefineFunction(callClosure,
                              datUniqletWith(&CLOSURE_DISPATCH, closure));
}


/*
 * Node evaluation
 */

/**
 * Executes a variable definition, by updating the given execution frame,
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = datTokenValue(varDef);
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
static void execFnDefs(Frame *frame, zvalue statements, zint start, zint end) {
    zint size = end - start;
    Closure *closures[size];

    for (zint i = 0; i < size; i++) {
        zvalue one = datListNth(statements, start + i);
        zvalue fnMap = datTokenValue(one);
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
    datTokenAssertType(closureNode, STR_CLOSURE);
    return buildClosure(NULL, frame, closureNode);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    datTokenAssertType(call, STR_CALL);
    call = datTokenValue(call);

    zvalue function = datMapGet(call, STR_FUNCTION);
    zvalue actuals = datMapGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(frame, function);

    zint argCount = datSize(actuals);
    zvalue args[argCount];

    // If there are any interpolated arguments, then `interpolate` is
    // set to `true`, and `fullCount` indicates the count of arguments
    // after interpolation.
    zint fullCount = 0;
    bool interpolate = false;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = datListNth(actuals, i);
        if (datTokenTypeIs(one, STR_INTERPOLATE)) {
            zvalue eval = execExpressionVoidOk(frame, datTokenValue(one));
            if (eval == NULL) {
                return NULL;
            } else if (!datTypeIs(eval, DAT_LIST)) {
                die("Attempt to interpolate non-list.");
            }
            args[i] = eval;
            fullCount += datSize(eval);
            interpolate = true;
        } else {
            zvalue eval = execExpressionVoidOk(frame, one);
            if (eval == NULL) {
                return NULL;
            }
            args[i] = eval;
            fullCount++;
        }
    }

    if (interpolate) {
        zvalue fullArgs[fullCount];
        zint at = 0;

        // Build the flattened argument list.
        for (zint i = 0; i < argCount; i++) {
            zvalue oneNode = datListNth(actuals, i);
            zvalue oneArg = args[i];
            if (datTokenTypeIs(oneNode, STR_INTERPOLATE)) {
                datArrayFromList(&fullArgs[at], oneArg);
                at += datSize(oneArg);
            } else {
                fullArgs[at] = oneArg;
                at++;
            }
        }

        return langCall(functionId, fullCount, fullArgs);
    } else {
        return langCall(functionId, argCount, args);
    }
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    datTokenAssertType(varRef, STR_VAR_REF);

    zvalue name = datTokenValue(varRef);
    return frameGet(frame, name);
}

/**
 * Executes an `interpolate` form.
 */
static zvalue execInterpolate(Frame *frame, zvalue interpolate) {
    datTokenAssertType(interpolate, STR_INTERPOLATE);

    zvalue result = execExpressionVoidOk(frame, datTokenValue(interpolate));

    if (result == NULL) {
        die("Attempt to interpolate void.");
    } else if (!datTypeIs(result, DAT_LIST)) {
        die("Attempt to interpolate non-list.");
    }

    zint size = datSize(result);
    switch (size) {
        case 0: {
            return NULL;
        }
        case 1: {
            return datListNth(result, 0);
        }
    }

    die("Attempt to interpolate multiple values.");
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    if (datTokenTypeIs(e, STR_LITERAL))
        return datTokenValue(e);
    else if (datTokenTypeIs(e, STR_VAR_REF))
        return execVarRef(frame, e);
    else if (datTokenTypeIs(e, STR_CALL))
        return execCall(frame, e);
    else if (datTokenTypeIs(e, STR_CLOSURE))
        return execClosure(frame, e);
    else if (datTokenTypeIs(e, STR_EXPRESSION))
        return execExpressionVoidOk(frame, datTokenValue(e));
    else if (datTokenTypeIs(e, STR_INTERPOLATE))
        return execInterpolate(frame, e);
    else {
        die("Invalid expression type.");
    }
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
