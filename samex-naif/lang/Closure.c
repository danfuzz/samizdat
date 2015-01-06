// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Closure construction and execution
//

#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

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
     * `ClosureNode` instance, which represents the fixed definition of the
     * closure.
     */
    zvalue node;
} ClosureInfo;

/**
 * Gets a pointer to the info of a closure value.
 */
static ClosureInfo *getInfo(zvalue closure) {
    return datPayload(closure);
}


//
// Module Definitions
//

// Documented in header.
zvalue exnoBuildClosure(zvalue node, Frame *frame) {
    zvalue result = datAllocValue(CLS_Closure, sizeof(ClosureInfo));
    ClosureInfo *info = getInfo(result);

    info->node = node;
    frameSnap(&info->frame, frame);
    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL_rest(Closure, call, args) {
    ClosureInfo *info = getInfo(ths);
    return exnoCallClosure(info->node, &info->frame, ths, args);
}

// Documented in header.
METH_IMPL_0(Closure, debugSymbol) {
    return METH_CALL(getInfo(ths)->node, debugSymbol);
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

    CLS_Closure = makeCoreClass(SYM(Closure), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Closure, call),
            METH_BIND(Closure, debugSymbol),
            METH_BIND(Closure, gcMark)));
}

// Documented in header.
zvalue CLS_Closure = NULL;
