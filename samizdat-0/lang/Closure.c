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
#include "type/Box.h"
#include "type/Generic.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"
#include "zlimits.h"


/*
 * ClosureDef Definitions
 */

/** Cache of all encountered `defMap`s, and their associated `ClosureDef`s. */
static zvalue defCacheBox = NULL;

/**
 * Repetition style of a formal argument.
 */
typedef enum {
    REP_NONE,
    REP_QMARK,
    REP_STAR,
    REP_PLUS
} zrepeat;

/**
 * Formal argument.
 */
typedef struct {
    /** Name (optional). */
    zvalue name;

    /** Repetition style. */
    zrepeat repeat;
} zformal;

/**
 * Cached info about a `defMap`.
 */
typedef struct {
    /**
     * Closure payload map that represents the fixed definition of the
     * closure. This can be the payload of either a `closure` or a
     * `fnDef` node.
     */
    zvalue defMap;

    /** The `"formals"` mapping of `defMap`, converted for easier use. */
    zformal formalz[LANG_MAX_FORMALS];

    /** The `"formals"` mapping inside `defMap`. */
    zvalue formals;

    /** The result of `collSize(formals)`. */
    zint formalsSize;

    /** The number of actual names in `formals`. */
    zint formalNameCount;

    /** The `"statements"` mapping inside `defMap`. */
    zvalue statements;

    /** The `"yield"` mapping inside `defMap`. */
    zvalue yield;

    /** The `"yieldDef"` mapping inside `defMap`. */
    zvalue yieldDef;
} ClosureDefInfo;

// TODO: Move to header.
extern zvalue TYPE_ClosureDef;

/**
 * Gets a pointer to the info of a `ClosureDef` value.
 */
static ClosureDefInfo *getDefInfo(zvalue closure) {
    return pbPayload(closure);
}

/**
 * Builds and returns a `ClosureDef` from a `closure` or `fnDef` payload.
 */
static zvalue buildClosureDef(zvalue defMap) {
    zvalue formals = collGet(defMap, STR_formals);
    zint formalsSize = collSize(formals);

    // Build out most of the result.

    zvalue result = pbAllocValue(TYPE_ClosureDef, sizeof(ClosureDefInfo));
    ClosureDefInfo *info = getDefInfo(result);

    info->defMap = defMap;
    info->formals = formals;
    info->formalsSize = formalsSize;
    info->formalNameCount = -1;
    info->statements = collGet(defMap, STR_statements);
    info->yield = collGet(defMap, STR_yield);
    info->yieldDef = collGet(defMap, STR_yieldDef);

    // Validate and transform all the formals.

    if (info->formalsSize > LANG_MAX_FORMALS) {
        die("Too many formals: %lld", info->formalsSize);
    }

    zvalue formalsArr[info->formalsSize];
    zvalue names = EMPTY_MAP;
    zint formalNameCount = 0;
    arrayFromList(formalsArr, formals);

    for (zint i = 0; i < info->formalsSize; i++) {
        zvalue formal = formalsArr[i];
        zvalue name = collGet(formal, STR_name);
        zvalue repeat = collGet(formal, STR_repeat);
        zrepeat rep;

        if (name != NULL) {
            if (collGet(names, name) != NULL) {
                die("Duplicate formal name: %s", valDebugString(name));
            }
            names = collPut(names, name, name);
            formalNameCount++;
        }

        if (repeat == NULL) {
            rep = REP_NONE;
        } else {
            if (collSize(repeat) != 1) {
                die("Invalid repeat modifier: %s", valDebugString(repeat));
            }
            switch (collNthChar(repeat, 0)) {
                case '*': rep = REP_STAR;  break;
                case '+': rep = REP_PLUS;  break;
                case '?': rep = REP_QMARK; break;
                default: {
                    die("Invalid repeat modifier: %s", valDebugString(repeat));
                }
            }
        }

        info->formalz[i].name = name;
        info->formalz[i].repeat = rep;
    }

    // All's well. Finish up.

    info->formalNameCount = formalNameCount;
    return result;
}

/**
 * Gets the `ClosureDef` associated with the given node.
 */
static zvalue getDef(zvalue node) {
    zvalue defMap = dataOf(node);
    zvalue cache = GFN_CALL(fetch, defCacheBox);
    zvalue result = collGet(cache, defMap);

    if (result == NULL) {
        result = buildClosureDef(defMap);
        cache = collPut(cache, defMap, result);
        GFN_CALL(store, defCacheBox, cache);
    }

    return result;
}

/* Documented in header. */
METH_IMPL(ClosureDef, gcMark) {
    zvalue closureDef = args[0];
    ClosureDefInfo *info = getDefInfo(closureDef);

    pbMark(info->defMap); // Everything else is also reachable from `defMap`.
    return NULL;
}

/* Documented in header. */
void langBindClosureDef(void) {
    TYPE_ClosureDef = coreTypeFromName(stringFromUtf8(-1, "ClosureDef"), true);
    METH_BIND(ClosureDef, gcMark);

    defCacheBox = makeMutableBox(EMPTY_MAP);
    pbImmortalize(defCacheBox);
}

/* Documented in header. */
zvalue TYPE_ClosureDef = NULL;


/*
 * Private Definitions
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

    /** `ClosureDef` containing all the vital info. */
    zvalue closureDef;
} ClosureInfo;

/**
 * Function call state. This is used during function call setup. Pointers
 * to these are passed directly within this file as well as passed
 * indirectly via the nonlocal exit handling code.
 */
typedef struct {
    /** Closure being called. */
    zvalue closure;

    /** Associated `ClosureDef`. */
    zvalue closureDef;

    /** Argument count. */
    zint argCount;

    /** Array of arguments. */
    const zvalue *args;
} CallState;


/**
 * Gets a pointer to the info of a closure value.
 */
static ClosureInfo *getInfo(zvalue closure) {
    return pbPayload(closure);
}

/**
 * Helper for evaluating `closure` and `fnDef` nodes. This allocates a
 * new `Closure` and sets up the `closureDef`.
 */
static zvalue buildClosure(zvalue node) {
    zvalue defMap = dataOf(node);
    zvalue formals = collGet(defMap, STR_formals);
    zint formalsSize = collSize(formals);

    // Build out most of the result.

    zvalue result = pbAllocValue(TYPE_Closure, sizeof(ClosureInfo));
    ClosureInfo *info = getInfo(result);

    info->closureDef = getDef(node);
    return result;
}

/**
 * Creates a variable map for all the formal arguments of the given
 * function.
 */
static zvalue bindArguments(zvalue closureDef, zint argCount,
        const zvalue *args) {
    ClosureDefInfo *info = getDefInfo(closureDef);
    zvalue formals = info->formals;
    zint formalsSize = info->formalsSize;

    if (formalsSize == 0) {
        if (argCount != 0) {
            die("Function called with too many arguments: %lld != 0",
                argCount);
        }
        return EMPTY_MAP;
    }

    //zvalue formalsArr[formalsSize];
    zmapping elems[info->formalNameCount];
    zint elemAt = 0;
    zint argAt = 0;

    //arrayFromList(formalsArr, formals);
    zformal *formalz = info->formalz;

    for (zint i = 0; i < formalsSize; i++) {
        //zvalue formal = formalsArr[i];
        zvalue name = formalz[i].name;
        //zvalue repeat = collGet(formal, STR_repeat);
        zrepeat repeat = formalz[i].repeat;
        bool ignore = (name == NULL);
        zvalue value;

        if (repeat != REP_NONE) {
            zint count;

            switch (repeat) {
                case REP_STAR: {
                    count = argCount - argAt;
                    break;
                }
                case REP_PLUS: {
                    if (argAt >= argCount) {
                        die("Function called with too few arguments "
                            "(plus argument): %lld",
                            argCount);
                    }
                    count = argCount - argAt;
                    break;
                }
                case REP_QMARK: {
                    count = (argAt >= argCount) ? 0 : 1;
                    break;
                }
                default: {
                    die("Invalid repeat enum (shouldn't happen).");
                }
            }

            value = ignore ? NULL : listFromArray(count, &args[argAt]);
            argAt += count;
        } else if (argAt >= argCount) {
            die("Function called with too few arguments: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        if (!ignore) {
            elems[elemAt].key = name;
            elems[elemAt].value = value;
            elemAt++;
        }
    }

    if (argAt != argCount) {
        die("Function called with too many arguments: %lld > %lld",
            argCount, argAt);
    }

    return mapFromArray(elemAt, elems);
}

/**
 * Helper that does the main work of `callClosure`, including nonlocal
 * exit binding when appropriate.
 */
static zvalue callClosureMain(CallState *callState, zvalue exitFunction) {
    zvalue closure = callState->closure;
    ClosureInfo *info = getInfo(closure);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    zvalue argMap = bindArguments(
        callState->closureDef, callState->argCount, callState->args);
    frameInit(&frame, &info->frame, closure, argMap);

    if (exitFunction != NULL) {
        frameAdd(&frame, info->yieldDef, exitFunction);
    }

    // Evaluate the statements, updating the frame as needed.

    zvalue statements = info->statements;
    zint statementsSize = collSize(statements);
    zvalue statementsArr[statementsSize];
    arrayFromList(statementsArr, statements);

    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = statementsArr[i];
        zvalue oneType = typeOf(one);

        // Switch on the first character of the type string to avoid
        // gratuitous `valEq` tests.
        switch (collNthChar(oneType, 0)) {
            case 'f': {
                if (valEq(oneType, STR_fnDef)) {
                    // Look for immediately adjacent `fnDef` nodes, and
                    // process them all together.
                    zint end = i + 1;
                    for (/*end*/; end < statementsSize; end++) {
                        zvalue one = statementsArr[end];
                        if (!hasType(one, STR_fnDef)) {
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
            case 'v': {
                if (valEq(oneType, STR_varDef)) {
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
        zvalue name = collGet(fnMap, STR_name);

        closures[i] = buildClosure(one);
        frameAdd(frame, name, closures[i]);
    }

    // Rewrite the local variable context of all the constructed closures
    // to be the *current* context. This allows for self-recursion when
    // `size == 1` and mutual recursion when `size > 1`.

    for (zint i = 0; i < size; i++) {
        frameSnap(&getInfo(closures[i])->frame, frame);
    }
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue execClosure(Frame *frame, zvalue closureNode) {
    zvalue result = buildClosure(closureNode);

    frameSnap(&getInfo(result)->frame, frame);
    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Closure, call) {
    // The first argument is the closure itself. The rest are the arguments
    // it is being called with, hence `argCount - 1, &args[1]` below.
    zvalue closure = args[0];
    ClosureInfo *info = getInfo(closure);
    CallState callState = { closure, info->closureDef, argCount - 1, &args[1] };
    zvalue result;

    if (info->yieldDef != NULL) {
        result = nleCall(callClosureWithNle, &callState);
    } else {
        result = callClosureMain(&callState, NULL);
    }

    return result;
}

/* Documented in header. */
METH_IMPL(Closure, canCall) {
    zvalue closure = args[0];
    zvalue value = args[1];
    ClosureInfo *info = getInfo(closure);

    return (info->formalsSize == 0) ? NULL : value;
}

/* Documented in header. */
METH_IMPL(Closure, debugString) {
    zvalue closure = args[0];
    ClosureInfo *info = getInfo(closure);
    zvalue name = collGet(info->defMap, STR_name);
    zvalue nameString = (name == NULL)
        ? stringFromUtf8(-1, "(unknown)")
        : GFN_CALL(debugString, name);

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@(Closure "),
        nameString,
        stringFromUtf8(-1, ")"));
}

/* Documented in header. */
METH_IMPL(Closure, gcMark) {
    zvalue closure = args[0];
    ClosureInfo *info = getInfo(closure);

    frameMark(&info->frame);
    pbMark(info->closureDef);
    return NULL;
}

/* Documented in header. */
void langBindClosure(void) {
    TYPE_Closure = coreTypeFromName(stringFromUtf8(-1, "Closure"), true);
    METH_BIND(Closure, call);
    METH_BIND(Closure, canCall);
    METH_BIND(Closure, debugString);
    METH_BIND(Closure, gcMark);

    langBindClosureDef();
}

/* Documented in header. */
zvalue TYPE_Closure = NULL;
