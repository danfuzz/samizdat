// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Concrete `Box` classes
//

#include "type/Box.h"
#include "type/List.h"
#include "type/Value.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Box state. Common across all the concrete subclasses.
 */
typedef struct {
    /** Content value. */
    zvalue value;

    /** True iff the box can be stored to (see spec for details). */
    bool canStore;
} BoxInfo;

/**
 * Gets a pointer to the value's info.
 */
static BoxInfo *getInfo(zvalue box) {
    return datPayload(box);
}

/**
 * Does the main action of fetching, without checking the argument class.
 */
static zvalue doFetch(zvalue box) {
    return getInfo(box)->value;
}

/**
 * Makes a box of the given class, with the given instance state.
 */
static zvalue newBox(zvalue cls, BoxInfo info) {
    zvalue result = datAllocValue(cls, sizeof(BoxInfo));
    *getInfo(result) = info;

    return result;
}


//
// Class Definition: `Box`
//

// Documented in spec.
METH_IMPL_0_1(Box, collect, function) {
    zvalue value = doFetch(ths);

    if ((value != NULL) && (function != NULL)) {
        value = FUN_CALL(function, value);
    }

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}

// Documented in spec.
METH_IMPL_0(Box, fetch) {
    return doFetch(ths);
}

// Documented in spec.
METH_IMPL_0(Box, gcMark) {
    BoxInfo *info = getInfo(ths);

    datMark(info->value);
    return NULL;
}

// Documented in spec.
METH_IMPL_1(Box, nextValue, out) {
    zvalue value = doFetch(ths);

    if (value != NULL) {
        cm_store(out, value);
        return EMPTY_LIST;
    } else {
        return NULL;
    }
}

/** Initializes the module. */
MOD_INIT(Box) {
    MOD_USE(Core);
    MOD_USE_NEXT(Cell);
    MOD_USE_NEXT(Promise);
    MOD_USE_NEXT(Result);

    CLS_Box = makeCoreClass(SYM(Box), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Box, collect),
            METH_BIND(Box, fetch),
            METH_BIND(Box, gcMark),
            METH_BIND(Box, nextValue)));
}

// Documented in header.
zvalue CLS_Box = NULL;


//
// Class Definition: `Cell`
//

// Documented in spec.
CMETH_IMPL_0_1(Cell, new, value) {
    return newBox(CLS_Cell, (BoxInfo) {value, true});
}

// Documented in spec.
METH_IMPL_0_1(Cell, store, value) {
    getInfo(ths)->value = value;
    return value;
}

/** Initializes the module. */
MOD_INIT(Cell) {
    MOD_USE(Box);

    CLS_Cell = makeCoreClass(SYM(Cell), CLS_Box,
        METH_TABLE(
            CMETH_BIND(Cell, new)),
        METH_TABLE(
            METH_BIND(Cell, store)));
}

// Documented in header.
zvalue CLS_Cell = NULL;


//
// Class Definition: `Promise`
//

// Documented in spec.
CMETH_IMPL_0(Promise, new) {
    return newBox(CLS_Promise, (BoxInfo) {NULL, true});
}

// Documented in spec.
METH_IMPL_0_1(Promise, store, value) {
    BoxInfo *info = getInfo(ths);

    if (!info->canStore) {
        die("Cannot `store()` to resolved `Promise`.");
    }

    info->canStore = false;
    info->value = value;
    return value;
}

/** Initializes the module. */
MOD_INIT(Promise) {
    MOD_USE(Box);

    CLS_Promise = makeCoreClass(SYM(Promise), CLS_Box,
        METH_TABLE(
            CMETH_BIND(Promise, new)),
        METH_TABLE(
            METH_BIND(Promise, store)));
}

// Documented in header.
zvalue CLS_Promise = NULL;


//
// Class Definition: `Result`
//

// Documented in spec.
CMETH_IMPL_0_1(Result, new, value) {
    return newBox(CLS_Result, (BoxInfo) {value, false});
}

// Documented in spec.
METH_IMPL_0_1(Result, store, value) {
    die("Cannot `store()` to `Result`.");
}

/** Initializes the module. */
MOD_INIT(Result) {
    MOD_USE(Box);

    CLS_Result = makeCoreClass(SYM(Result), CLS_Box,
        METH_TABLE(
            CMETH_BIND(Result, new)),
        METH_TABLE(
            METH_BIND(Result, store)));
}

// Documented in header.
zvalue CLS_Result = NULL;
