// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `ExecNode` class
//
// Instances are translated from the (per spec) executable tree node form.
// These are what are used when actually running the interpreter.

#include "const.h"
#include "type/define.h"

#include "impl.h"
#include "langnode.h"


//
// Private Definitions
//

/**
 * Payload data. This is a approximately a union (in the math sense, not the C
 * sense) of all the possible named bindings of any executable node type.
 * Bound values which are themselves executable nodes are recursively
 * translated. Other values are in some cases left alone and in others
 * translated to a more convenient form for execution.
 */
typedef struct {
    /** Original node. */
    zvalue node;

    /** Type of the node. */
    zevalType type;

    /** `node::box`. */
    zevalType box;

    /** `node::formals`. */
    zvalue formals;

    /** `node::name`. */
    zvalue name;

    /** `node::statements`. */
    zvalue statements;

    /** `node::target`. */
    zvalue target;

    /** `node::value`. */
    zvalue value;

    /** `node::values`. */
    zvalue values;

    /** `node::yield`. */
    zvalue yield;

    /** `node::yieldDef`. */
    zvalue yieldDef;
} ExecNodeInfo;

/**
 * Gets the info of a record.
 */
static ExecNodeInfo *getInfo(zvalue value) {
    return (ExecNodeInfo *) datPayload(value);
}


//
// Module Definitions
//

// TODO


//
// Class Definition
//

// Documented in header.
METH_IMPL_0(ExecNode, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(ExecNode, gcMark) {
    ExecNodeInfo *info = getInfo(ths);

    // `statements` might not be covered by `node` (when it was generated via
    // translation of an `import*`), but everything else is.
    datMark(info->node);
    datMark(info->statements);

    return NULL;
}

/** Initializes the module. */
MOD_INIT(ExecNode) {
    MOD_USE(cls);

    CLS_ExecNode = makeCoreClass(symbolFromUtf8(-1, "ExecNode"), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(ExecNode, debugSymbol),
            METH_BIND(ExecNode, gcMark)));
}

// Documented in header.
zvalue CLS_ExecNode = NULL;
