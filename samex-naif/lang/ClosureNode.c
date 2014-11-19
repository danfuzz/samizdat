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
 * Payload data.
 */
typedef struct {
    /** `node::formals`. */
    zvalue formals;

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


//
// Module Definitions
//

// Documented in header.
zvalue exnoExecuteClosure(zvalue node) {
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

    if (!recGet3(orig,
            SYM(formals),    &info->formals,
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
    return result;
}

// Documented in spec.
METH_IMPL_0(ClosureNode, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(ClosureNode, gcMark) {
    ClosureNodeInfo *info = getInfo(ths);

    datMark(info->formals);
    datMark(info->name);
    datMark(info->statements);
    datMark(info->yield);
    datMark(info->yieldDef);

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
