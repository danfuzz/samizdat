// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `ClosureNode` class
//
// Translation of the main info of a `closure` node.

#include "const.h"
#include "type/define.h"
#include "type/List.h"

#include "impl.h"
#include "langnode.h"


//
// Private Definitions
//

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
 * Payload data. This corresponds with the payload of a `closure` node, but
 * with `formals` reworked to be easier to digest.
 */
typedef struct {
    /** The `node::formals`, converted for easier use. */
    zformal formals[LANG_MAX_FORMALS];

    /** The result of `get_size(formals)`. */
    zint formalsSize;

    /** The number of actual names in `formals`, plus one for a `yieldDef`. */
    zint formalsNameCount;

    /** `node::name`. */
    zvalue name;

    /** `node::statements`. */
    zvalue statements;

    /** `node::yield`. */
    zvalue yield;

    /** `node::yieldDef`. */
    zvalue yieldDef;
} ClosureNodeInfo;

/**
 * Gets the info of a record.
 */
static ClosureNodeInfo *getInfo(zvalue value) {
    return (ClosureNodeInfo *) datPayload(value);
}

/**
 * Converts the given `formals`, storing the result in the given `info`.
 */
static void convertFormals(ClosureNodeInfo *info, zvalue formalsList) {
    zarray formals = zarrayFromList(formalsList);

    if (formals.size > LANG_MAX_FORMALS) {
        die("Too many formals: %lld", formals.size);
    }

    zvalue names[formals.size + 1];  // For detecting duplicates.
    zint nameCount = 0;

    if (info->yieldDef != NULL) {
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

    info->formalsSize = formals.size;
    info->formalsNameCount = nameCount;
}


//
// Module Definitions
//

// Documented in header.
zvalue exnoBuildClosure(zvalue node, Frame *frame) {
    // TODO! FIXME!
    die("TODO");
}

// Documented in header.
zvalue exnoCallClosure(zvalue node, Frame *frame, zarray args) {
    // TODO! FIXME!
    die("TODO");
}


//
// Class Definition
//

/**
 * Constructs an instance from the given (per spec) `closure` tree node.
 */
CMETH_IMPL_1(ClosureNode, new, orig) {
    zvalue result = datAllocValue(CLS_ClosureNode, sizeof(ClosureNodeInfo));
    ClosureNodeInfo *info = getInfo(result);
    zvalue formals;

    if (!recGet3(orig,
            SYM(formals),    &formals,
            SYM(statements), &info->statements,
            SYM(yield),      &info->yield)) {
        die("Invalid `closure` node.");
    }

    // These are both optional.
    recGet2(orig,
        SYM(name),     &info->name,
        SYM(yieldDef), &info->yieldDef);

    exnoConvert(&info->statements);
    exnoConvert(&info->yield);
    convertFormals(info, formals);

    return result;
}

// Documented in spec.
METH_IMPL_0(ClosureNode, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(ClosureNode, gcMark) {
    ClosureNodeInfo *info = getInfo(ths);

    datMark(info->name);
    datMark(info->statements);
    datMark(info->yield);
    datMark(info->yieldDef);

    for (zint i = 0; i < info->formalsSize; i++) {
        datMark(info->formals[i].name);
    }

    return NULL;
}

/** Initializes the module. */
MOD_INIT(ClosureNode) {
    MOD_USE(cls);

    CLS_ClosureNode = makeCoreClass(
        symbolFromUtf8(-1, "ClosureNode"), CLS_Core,
        METH_TABLE(
            CMETH_BIND(ClosureNode, new)),
        METH_TABLE(
            METH_BIND(ClosureNode, debugSymbol),
            METH_BIND(ClosureNode, gcMark)));
}

// Documented in header.
zvalue CLS_ClosureNode = NULL;
