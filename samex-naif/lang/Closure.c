// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Closure construction and execution
//

#include "const.h"
#include "type/Box.h"
#include "type/Record.h"
#include "type/Jump.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

enum {
    /** Whether to spew to the console about cache behavior. */
    CHATTY_CACHEY = false
};

/**
 * Cache that maps all encountered closure(ish) nodes to their associated
 * `Closure` values.
 */
static zvalue nodeCache = NULL;

/**
 * Box for `nodeCache`. This is registered as an immortal, so that
 * whatever `nodeCache` happens to point at will never be garbage.
 */
static zvalue nodeCacheBox = NULL;

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
     * Snapshot of the frame that was active at the moment the closure was
     * constructed. Left uninitialized for cached values.
     */
    Frame frame;

    /**
     * Closure payload map that represents the fixed definition of the
     * closure.
     */
    zvalue defMap;

    /** The `"formals"` mapping of `defMap`, converted for easier use. */
    zformal formals[LANG_MAX_FORMALS];

    /** The result of `get_size(formals)`. */
    zint formalsSize;

    /** The number of actual names in `formals`, plus one for a `yieldDef`. */
    zint formalNameCount;

    /** The `"statements"` mapping inside `defMap`. */
    zvalue statements;

    /** The `"yield"` mapping inside `defMap`. */
    zvalue yield;

    /** The `"yieldDef"` mapping inside `defMap`. */
    zvalue yieldDef;
} ClosureInfo;

/**
 * Gets a pointer to the info of a closure value.
 */
static ClosureInfo *getInfo(zvalue closure) {
    return datPayload(closure);
}

/**
 * Builds and returns a `Closure` from a `closure` payload, suitable for
 * storage in the cache.
 */
static zvalue buildCachedClosure(zvalue defMap) {
    zvalue formals = get(defMap, SYM_formals);
    zint formalsSize = get_size(formals);

    // Build out most of the result.

    zvalue result = datAllocValue(CLS_Closure, sizeof(ClosureInfo));
    ClosureInfo *info = getInfo(result);

    info->defMap = defMap;
    info->formalsSize = formalsSize;
    info->statements = get(defMap, SYM_statements);
    info->yield = get(defMap, SYM_yield);
    info->yieldDef = get(defMap, SYM_yieldDef);

    // Validate and transform all the formals.

    if (formalsSize > LANG_MAX_FORMALS) {
        die("Too many formals: %lld", formalsSize);
    }

    zvalue formalsArr[formalsSize];
    zvalue names = EMPTY_MAP;
    zint formalNameCount = 0;
    arrayFromList(formalsArr, formals);

    for (zint i = 0; i < formalsSize; i++) {
        zvalue formal = formalsArr[i];
        zvalue name = get(formal, SYM_name);
        zvalue repeat = get(formal, SYM_repeat);
        zrepeat rep;

        if (name != NULL) {
            if (get(names, name) != NULL) {
                die("Duplicate formal name: %s", cm_debugString(name));
            }
            names = METH_CALL(put, names, name, name);
            formalNameCount++;
        }

        if (repeat == NULL) {
            rep = REP_NONE;
        } else {
            switch (symbolEvalType(repeat)) {
                case EVAL_CH_STAR:  { rep = REP_STAR;  break; }
                case EVAL_CH_PLUS:  { rep = REP_PLUS;  break; }
                case EVAL_CH_QMARK: { rep = REP_QMARK; break; }
                default: {
                    die("Invalid repeat modifier: %s", cm_debugString(repeat));
                }
            }
        }

        info->formals[i].name = name;
        info->formals[i].repeat = rep;
    }

    // All's well. Finish up.

    info->formalNameCount = formalNameCount + (info->yieldDef ? 1 : 0);
    return result;
}

/**
 * Gets the cached `Closure` associated with the given node.
 */
static zvalue getCachedClosure(zvalue node) {
    zvalue result = get(nodeCache, node);

    if (CHATTY_CACHEY) {
        static int hits = 0;
        static int total = 0;
        if (result != NULL) {
            hits++;
        }
        total++;
        if ((total % 1000000) == 0) {
            note("Closure Cache: Hit rate %d/%d == %5.2f%%; %d unique nodes.",
                hits, total, (100.0 * hits) / total, total - hits);
        }
    }

    if (result == NULL) {
        result = buildCachedClosure(get_data(node));
        nodeCache = METH_CALL(put, nodeCache, node, result);
        METH_CALL(store, nodeCacheBox, nodeCache);
    }

    return result;
}

/**
 * Creates a variable map for all the formal arguments of the given
 * function.
 */
static zvalue bindArguments(zvalue closure, zvalue exitFunction,
        zint argCount, const zvalue *args) {
    ClosureInfo *info = getInfo(closure);
    zmapping elems[info->formalNameCount];
    zformal *formals = info->formals;
    zint formalsSize = info->formalsSize;
    zint elemAt = 0;
    zint argAt = 0;

    for (zint i = 0; i < formalsSize; i++) {
        zvalue name = formals[i].name;
        zrepeat repeat = formals[i].repeat;
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
            elems[elemAt].value = makeResult(value);
            elemAt++;
        }
    }

    if (argAt != argCount) {
        die("Function called with too many arguments: %lld > %lld",
            argCount, argAt);
    }

    if (exitFunction != NULL) {
        elems[elemAt].key = info->yieldDef;
        elems[elemAt].value = makeResult(exitFunction);
        elemAt++;
    }

    return symbolTableFromArray(elemAt, elems);
}

/**
 * Helper for evaluating `closure` nodes. This allocates a new `Closure`,
 * cloning its info from a cached instance.
 */
static zvalue buildClosure(zvalue node) {
    zvalue cachedClosure = getCachedClosure(node);
    zvalue result = datAllocValue(CLS_Closure, sizeof(ClosureInfo));

    utilCpy(ClosureInfo, getInfo(result), getInfo(cachedClosure), 1);
    return result;
}

/**
 * Helper that does the main work of `callClosure`, including nonlocal
 * exit binding when appropriate.
 */
static zvalue callClosureMain(zvalue closure, zvalue exitFunction,
        zint argCount, const zvalue *args) {
    ClosureInfo *info = getInfo(closure);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    zvalue argTable = bindArguments(closure, exitFunction, argCount, args);
    frameInit(&frame, &info->frame, closure, argTable);

    // Evaluate the statements, updating the frame as needed.
    execStatements(&frame, info->statements);

    // Evaluate the yield expression, and return the final result.
    return execExpressionOrMaybe(&frame, info->yield);
}


//
// Module Definitions
//

// Documented in header.
zvalue execClosure(Frame *frame, zvalue closureNode) {
    zvalue result = buildClosure(closureNode);

    frameSnap(&getInfo(result)->frame, frame);
    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL_rest(Closure, call, args) {
    if (getInfo(ths)->yieldDef == NULL) {
        return callClosureMain(ths, NULL, argsSize, args);
    }

    zvalue jump = makeJump();
    jumpArm(jump);

    zvalue result = callClosureMain(ths, jump, argsSize, args);
    jumpRetire(jump);

    return result;
}

// Documented in header.
METH_IMPL_0(Closure, debugSymbol) {
    return get(getInfo(ths)->defMap, SYM_name);
}

// Documented in header.
METH_IMPL_0(Closure, gcMark) {
    ClosureInfo *info = getInfo(ths);

    frameMark(&info->frame);
    datMark(info->defMap);  // All the other bits are derived from this.
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Closure) {
    MOD_USE(Box);

    CLS_Closure = makeCoreClass(symbolFromUtf8(-1, "Closure"), CLS_Core,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Closure, call),
            METH_BIND(Closure, debugSymbol),
            METH_BIND(Closure, gcMark),
            NULL));

    nodeCache = EMPTY_MAP;
    nodeCacheBox = datImmortalize(makeCell(EMPTY_MAP));
}

// Documented in header.
zvalue CLS_Closure = NULL;
