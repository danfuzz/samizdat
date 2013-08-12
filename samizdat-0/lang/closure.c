/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Closure construction and execution
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


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
static UniqletInfoDispatch CLOSURE_DISPATCH = {
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

// Defined below.
static void execFnDefs(Frame *frame, zint size, const zvalue *statements);

/**
 * Binds variables for all the formal arguments of the given
 * function (if any), into the given execution frame.
 */
static void bindArguments(Frame *frame, zvalue node,
                          zint argCount, const zvalue *args) {
    zvalue formals = mapGet(node, STR_FORMALS);

    if (formals == NULL) {
        return;
    }

    zint formalsSize = pbSize(formals);
    zvalue formalsArr[formalsSize];
    zint argAt = 0;

    arrayFromList(formalsArr, formals);

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = formalsArr[i];
        zvalue name = mapGet(formal, STR_NAME);
        zvalue repeat = mapGet(formal, STR_REPEAT);
        bool ignore = (name == NULL);
        zvalue value;

        if (repeat != NULL) {
            zint count;

            if ((pbSize(repeat) != 1) || !hasType(repeat, TYPE_String)) {
                die("Invalid repeat modifier (non-string).");
            }

            switch (stringNth(repeat, 0)) {
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
                value = ignore ? NULL : listFromArray(count, &args[argAt]);
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

    zvalue statements = mapGet(defMap, STR_STATEMENTS);
    zvalue yield = mapGet(defMap, STR_YIELD);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    frameInit(&frame, &closure->frame, closureValue, EMPTY_MAP);
    bindArguments(&frame, defMap, argCount, args);

    if (exitFunction != NULL) {
        zvalue name = mapGet(defMap, STR_YIELD_DEF);
        frameAdd(&frame, name, exitFunction);
    }

    // Evaluate the statements, updating the frame as needed.

    zint statementsSize = pbSize(statements);
    zvalue statementsArr[statementsSize];
    arrayFromList(statementsArr, statements);

    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = statementsArr[i];
        zvalue oneType = typeOf(one);

        // Switch on size of type string to avoid gratuitous `pbEq` tests.
        switch (pbSize(oneType)) {
            case 5: {
                if (pbEq(oneType, STR_FN_DEF)) {
                    // Look for immediately adjacent `fnDef` nodes, and
                    // process them all together.
                    zint end = i + 1;
                    for (/*end*/; end < statementsSize; end++) {
                        zvalue one = statementsArr[end];
                        if (!hasType(one, STR_FN_DEF)) {
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
                if (pbEq(oneType, STR_VAR_DEF)) {
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
    Closure *closure = uniqletGetState(state, &CLOSURE_DISPATCH);
    CallState callState = { state, closure, argCount, args };

    zvalue result;

    if (mapGet(closure->defMap, STR_YIELD_DEF) != NULL) {
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
    zvalue defMap = dataOf(node);

    frameSnap(&closure->frame, frame);
    closure->defMap = defMap;

    if (resultClosure != NULL) {
        *resultClosure = closure;
    }

    return fnFrom(
        0, -1,
        callClosure,
        uniqletFrom(&CLOSURE_DISPATCH, closure),
        mapGet(defMap, STR_NAME));
}

/**
 * Executes a sequence of one or more statement-level function definitions,
 * from the `start` index (inclusive) to the `end` index (exclusive) in the
 * given `statements` array.
 */
static void execFnDefs(Frame *frame, zint size, const zvalue *statements) {
    Closure *closures[size];

    for (zint i = 0; i < size; i++) {
        zvalue one = statements[i];
        zvalue fnMap = dataOf(one);
        zvalue name = mapGet(fnMap, STR_NAME);
        frameAdd(frame, name, buildClosure(&closures[i], frame, one));
    }

    // Rewrite the local variable context of all the constructed closures
    // to be the *current* context. This allows for self-recursion when
    // `size == 1` and mutual recursion when `size > 1`.

    for (zint i = 0; i < size; i++) {
        frameSnap(&closures[i]->frame, frame);
    }
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue execClosure(Frame *frame, zvalue closureNode) {
    return buildClosure(NULL, frame, closureNode);
}
