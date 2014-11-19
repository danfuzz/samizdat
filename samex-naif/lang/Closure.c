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
 * Cached info about a `closure`.
 */
typedef struct {
    /**
     * Snapshot of the frame that was active at the moment the closure was
     * constructed. Left uninitialized for cached values.
     */
    Frame frame;

    /**
     * Original closure node, which represents the fixed definition of the
     * closure.
     */
    zvalue node;

    /** The `"formals"` mapping of `node`, converted for easier use. */
    zformal formals[LANG_MAX_FORMALS];

    /** The result of `get_size(formals)`. */
    zint formalsSize;

    /** The number of actual names in `formals`, plus one for a `yieldDef`. */
    zint formalNameCount;

    /** The `"statements"` mapping inside `node`. */
    zvalue statements;

    /** The `"yield"` mapping inside `node`. */
    zvalue yield;

    /** The `"yieldDef"` mapping inside `node`. */
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
static zvalue buildCachedClosure(zvalue node) {
    // Build out most of the result.

    zvalue result = datAllocValue(CLS_Closure, sizeof(ClosureInfo));
    ClosureInfo *info = getInfo(result);

    zvalue formalsList;
    recGet4(node,
        SYM(formals),    &formalsList,
        SYM(statements), &info->statements,
        SYM(yield),      &info->yield,
        SYM(yieldDef),   &info->yieldDef);

    zarray formals = zarrayFromList(formalsList);

    info->node = node;
    info->formalsSize = formals.size;

    // Validate and transform all the formals.

    if (formals.size > LANG_MAX_FORMALS) {
        die("Too many formals: %lld", formals.size);
    }

    zvalue names[formals.size + 1];  // For detecting duplicates.
    zint nameCount = 0;

    if (info->yieldDef) {
        names[0] = info->yieldDef;
        nameCount = 1;
    }

    for (zint i = 0; i < formals.size; i++) {
        zvalue formal = formals.elems[i];
        zvalue name, repeat;
        zrepeat rep;

        symtabGet2(formal, SYM(name), &name, SYM(repeat), &repeat);

        if (name != NULL) {
            names[nameCount] = name;
            nameCount++;
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

        info->formals[i] = (zformal) {.name = name, .repeat = rep};
    }

    // Detect duplicate formal argument names.

    if (nameCount > 1) {
        symbolSort(nameCount, names);
        for (zint i = 1; i < nameCount; i++) {
            if (names[i - 1] == names[i]) {
                die("Duplicate formal name: %s", cm_debugString(names[i]));
            }
        }
    }

    // All's well. Finish up.

    info->formalNameCount = nameCount;
    return result;
}

/**
 * Gets the cached `Closure` associated with the given node.
 */
static zvalue getCachedClosure(zvalue node) {
    zvalue result = cm_get(nodeCache, node);

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
        result = buildCachedClosure(node);
        nodeCache = cm_cat(nodeCache,
            mapFromMapping((zmapping) {node, result}));
        cm_store(nodeCacheBox, nodeCache);
    }

    return result;
}

/**
 * Creates a variable map for all the formal arguments of the given
 * function.
 */
static zvalue bindArguments(zvalue closure, zvalue exitFunction, zarray args) {
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
                    count = args.size - argAt;
                    break;
                }
                case REP_PLUS: {
                    if (argAt >= args.size) {
                        die("Function called with too few arguments "
                            "(plus argument): %lld",
                            args.size);
                    }
                    count = args.size - argAt;
                    break;
                }
                case REP_QMARK: {
                    count = (argAt >= args.size) ? 0 : 1;
                    break;
                }
                default: {
                    die("Invalid repeat enum (shouldn't happen).");
                }
            }

            value = ignore
                ? NULL
                : listFromZarray((zarray) {count, &args.elems[argAt]});
            argAt += count;
        } else if (argAt >= args.size) {
            die("Function called with too few arguments: %lld", args.size);
        } else {
            value = args.elems[argAt];
            argAt++;
        }

        if (!ignore) {
            elems[elemAt].key = name;
            elems[elemAt].value = cm_new(Result, value);
            elemAt++;
        }
    }

    if (argAt != args.size) {
        die("Function called with too many arguments: %lld > %lld",
            args.size, argAt);
    }

    if (exitFunction != NULL) {
        elems[elemAt].key = info->yieldDef;
        elems[elemAt].value = cm_new(Result, exitFunction);
        elemAt++;
    }

    return symtabFromZassoc((zassoc) {elemAt, elems});
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
        zarray args) {
    ClosureInfo *info = getInfo(closure);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    zvalue argTable = bindArguments(closure, exitFunction, args);
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
        return callClosureMain(ths, NULL, args);
    }

    zvalue jump = makeJump();
    jumpArm(jump);

    zvalue result = callClosureMain(ths, jump, args);
    jumpRetire(jump);

    return result;
}

// Documented in header.
METH_IMPL_0(Closure, debugSymbol) {
    return cm_get(getInfo(ths)->node, SYM(name));
}

// Documented in header.
METH_IMPL_0(Closure, gcMark) {
    ClosureInfo *info = getInfo(ths);

    frameMark(&info->frame);
    datMark(info->node);  // All the other bits are derived from this.
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Closure) {
    MOD_USE(cls);
    MOD_USE(Jump);

    CLS_Closure = makeCoreClass(symbolFromUtf8(-1, "Closure"), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Closure, call),
            METH_BIND(Closure, debugSymbol),
            METH_BIND(Closure, gcMark)));

    nodeCache = EMPTY_MAP;
    nodeCacheBox = datImmortalize(cm_new(Cell, EMPTY_MAP));
}

// Documented in header.
zvalue CLS_Closure = NULL;
