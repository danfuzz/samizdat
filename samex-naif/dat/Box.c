// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Box manipulation
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
 * Box state.
 */
typedef struct {
    /** Content value. */
    zvalue value;

    /** True iff this is a set-once (yield) box. */
    bool setOnce;

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
 * Does the main action of storing, without checking the argument class.
 */
static zvalue doStore(zvalue box, zvalue value) {
    BoxInfo *info = getInfo(box);

    if (!info->canStore) {
        die("Attempt to store to fixed box.");
    }

    if (info->setOnce) {
        info->canStore = false;
    }

    info->value = value;
    return value;
}


//
// Exported Definitions
//

// Documented in header.
zvalue makeCell(zvalue value) {
    zvalue result = datAllocValue(CLS_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = value;
    info->canStore = true;
    info->setOnce = false;

    return result;
}

// Documented in header.
zvalue makePromise(void) {
    zvalue result = datAllocValue(CLS_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->canStore = true;
    info->setOnce = true;

    return result;
}

// Documented in header.
zvalue makeResult(zvalue value) {
    zvalue result = datAllocValue(CLS_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = value;
    info->canStore = false;
    info->setOnce = true;

    return result;
}


//
// Class Definition
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

// Documented in spec.
METH_IMPL_0_1(Box, store, value) {
    return doStore(ths, value);
}

/** Initializes the module. */
MOD_INIT(Box) {
    MOD_USE(Generator);
    MOD_USE(Core);

    CLS_Box = makeCoreClass(SYM(Box), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Box, collect),
            METH_BIND(Box, fetch),
            METH_BIND(Box, gcMark),
            METH_BIND(Box, nextValue),
            METH_BIND(Box, store)));
}

// Documented in header.
zvalue CLS_Box = NULL;
