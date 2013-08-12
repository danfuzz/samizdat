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


/*
 * Helper definitions
 */

// Defined below.
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

    /** Ordering id. */
    zint orderId;

    /** The `"formals"` mapping inside `defMap`. */
    zvalue formals;

    /** The `"statements"` mapping inside `defMap`. */
    zvalue statements;

    /** The `"yield"` mapping inside `defMap`. */
    zvalue yield;

    /** The `"yieldDef"` mapping inside `defMap`. */
    zvalue yieldDef;
} ClosureInfo;

/**
 * Function call state. This is used during function call setup. Pointers
 * to these are passed directly within this file as well as passed
 * indirectly via the nonlocal exit handling code.
 */
typedef struct {
    /** Closure being called. */
    zvalue closure;

    /** Argument count. */
    zint argCount;

    /** Array of arguments. */
    const zvalue *args;
} CallState;


/**
 * Gets a pointer to the info of a closure value.
 */
static ClosureInfo *closureInfo(zvalue closure) {
    return pbPayload(closure);
}

/**
 * Helper for evaluating `closure` and `fnDef` nodes. This allocates a
 * new `Closure` and sets up everything but the `frame`.
 */
static zvalue buildClosure(zvalue node) {
    zvalue result = pbAllocValue(TYPE_Closure, sizeof(ClosureInfo));
    ClosureInfo *info = closureInfo(result);
    zvalue defMap = dataOf(node);

    info->defMap = defMap;
    info->formals = mapGet(defMap, STR_FORMALS);
    info->statements = mapGet(defMap, STR_STATEMENTS);
    info->yield = mapGet(defMap, STR_YIELD);
    info->yieldDef = mapGet(defMap, STR_YIELD_DEF);
    info->orderId = pbOrderId();

    return result;
}

/**
 * Binds variables for all the formal arguments of the given
 * function (if any), into the given execution frame.
 */
static void bindArguments(Frame *frame, zvalue closure,
        zint argCount, const zvalue *args) {
    ClosureInfo *info = closureInfo(closure);
    zvalue formals = info->formals;
    zint formalsSize = (formals == NULL) ? 0 : pbSize(formals);
    zvalue formalsArr[formalsSize];
    zint argAt = 0;

    if (formalsSize != 0) {
        arrayFromList(formalsArr, formals);
    }

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
    zvalue closure = callState->closure;
    ClosureInfo *info = closureInfo(closure);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    frameInit(&frame, &info->frame, closure, EMPTY_MAP);
    bindArguments(&frame, closure, callState->argCount, callState->args);

    if (exitFunction != NULL) {
        frameAdd(&frame, info->yieldDef, exitFunction);
    }

    // Evaluate the statements, updating the frame as needed.

    zvalue statements = info->statements;
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

    zvalue yield = info->yield;
    return (yield == NULL) ? NULL : execExpressionVoidOk(&frame, yield);
}

/**
 * Nonlocal exit callthrough function. This is called by `nleCall`.
 */
static zvalue callClosureWithNle(void *state, zvalue exitFunction) {
    return callClosureMain((CallState *) state, exitFunction);
}

/**
 * Executes a sequence of one or more statement-level function definitions.
 * Each of the elements of `statements` must be a `fnDef` node.
 */
static void execFnDefs(Frame *frame, zint size, const zvalue *statements) {
    zvalue closures[size];

    for (zint i = 0; i < size; i++) {
        zvalue one = statements[i];
        zvalue fnMap = dataOf(one);
        zvalue name = mapGet(fnMap, STR_NAME);

        closures[i] = buildClosure(one);
        frameAdd(frame, name, closures[i]);
    }

    // Rewrite the local variable context of all the constructed closures
    // to be the *current* context. This allows for self-recursion when
    // `size == 1` and mutual recursion when `size > 1`.

    for (zint i = 0; i < size; i++) {
        frameSnap(&closureInfo(closures[i])->frame, frame);
    }
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue execClosure(Frame *frame, zvalue closureNode) {
    zvalue result = buildClosure(closureNode);

    frameSnap(&closureInfo(result)->frame, frame);
    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Closure_call(zint argCount, const zvalue *args) {
    // The first argument is the closure itself. The rest are the arguments
    // it is being called with, hence `argCount - 1, &args[1]` below.
    zvalue closure = args[0];
    ClosureInfo *info = closureInfo(closure);
    CallState callState = { closure, argCount - 1, &args[1] };
    zvalue result;

    if (info->yieldDef != NULL) {
        result = nleCall(callClosureWithNle, &callState);
    } else {
        result = callClosureMain(&callState, NULL);
    }

    return result;
}

/* Documented in header. */
static zvalue Closure_canCall(zint argCount, const zvalue *args) {
    zvalue closure = args[0];
    zvalue value = args[1];
    ClosureInfo *info = closureInfo(closure);

    // This closure can be called with an argument as long as it defines
    // at least one formal. `formals` is either `NULL` or non-empty, hence
    // the test.
    return (info->formals == NULL) ? NULL : value;
}

/* Documented in header. */
static zvalue Closure_debugString(zint argCount, const zvalue *args) {
    zvalue closure = args[0];
    ClosureInfo *info = closureInfo(closure);
    zvalue name = mapGet(info->defMap, STR_NAME);

    zvalue result = stringFromUtf8(-1, "@(Closure ");

    if (name != NULL) {
        result = stringAdd(result, fnCall(GFN_debugString, 1, &name));
    } else {
        result = stringAdd(result, stringFromUtf8(-1, "(unknown)"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
static zvalue Closure_gcMark(zint argCount, const zvalue *args) {
    zvalue closure = args[0];
    ClosureInfo *info = closureInfo(closure);

    frameMark(&info->frame);
    pbMark(info->defMap);
    return NULL;
}

/* Documented in header. */
static zvalue Closure_order(zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (closureInfo(v1)->orderId < closureInfo(v2)->orderId)
        ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void langBindClosure(void) {
    TYPE_Closure = coreTypeFromName(stringFromUtf8(-1, "Closure"));
    gfnBindCore(GFN_call,        TYPE_Closure, Closure_call);
    gfnBindCore(GFN_canCall,     TYPE_Closure, Closure_canCall);
    gfnBindCore(GFN_debugString, TYPE_Closure, Closure_debugString);
    gfnBindCore(GFN_gcMark,      TYPE_Closure, Closure_gcMark);
    gfnBindCore(GFN_order,       TYPE_Closure, Closure_order);
}

/* Documented in header. */
zvalue TYPE_Closure = NULL;
