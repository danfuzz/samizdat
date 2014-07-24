// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Box manipulation
//

#include "type/Box.h"
#include "type/Generic.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Class.h"
#include "type/Value.h"
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
 * Does the main action of fetching, without checking the argument type.
 */
static zvalue doFetch(zvalue box) {
    zvalue result = getInfo(box)->value;

    // The box has a value that we are about to return. Since the box
    // could become garbage after this, we have to treat the value as
    // "escaped" and so explicitly add the result value to the frame at
    // this point. This ensures that GC will be able to find it.
    return datFrameAdd(result);
}

/**
 * Does the main action of storing, without checking the argument type.
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
zvalue boxStoreNullOk(zvalue box, zvalue value) {
    return (value == NULL)
        ? GFN_CALL(store, box)
        : GFN_CALL(store, box, value);
}

// Documented in header.
zvalue makeCell(zvalue value) {
    zvalue result = datAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = value;
    info->canStore = true;
    info->setOnce = false;

    return result;
}

// Documented in header.
zvalue makePromise(void) {
    zvalue result = datAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->canStore = true;
    info->setOnce = true;

    return result;
}

// Documented in header.
zvalue makeResult(zvalue value) {
    zvalue result = datAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = value;
    info->canStore = false;
    info->setOnce = true;

    return result;
}


//
// Type Definition
//

// Documented in header.
METH_IMPL(Box, collect) {
    zvalue box = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;
    zvalue value = doFetch(box);

    if ((value != NULL) && (function != NULL)) {
        value = FUN_CALL(function, value);
    }

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}

// Documented in header.
METH_IMPL(Box, fetch) {
    zvalue box = args[0];
    return doFetch(box);
}

// Documented in header.
METH_IMPL(Box, gcMark) {
    zvalue box = args[0];
    BoxInfo *info = getInfo(box);

    datMark(info->value);
    return NULL;
}

// Documented in header.
METH_IMPL(Box, nextValue) {
    zvalue box = args[0];
    zvalue outBox = args[1];
    zvalue value = doFetch(box);

    if (value != NULL) {
        GFN_CALL(store, outBox, value);
        return EMPTY_LIST;
    } else {
        return NULL;
    }
}

// Documented in header.
METH_IMPL(Box, store) {
    zvalue box = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;
    return doStore(box, value);
}

/** Initializes the module. */
MOD_INIT(Box) {
    MOD_USE(Generator);
    MOD_USE(Value);

    GFN_store = makeGeneric(1, 2, GFN_NONE, stringFromUtf8(-1, "store"));
    datImmortalize(GFN_store);

    TYPE_Box = makeCoreType(stringFromUtf8(-1, "Box"), TYPE_Value);

    METH_BIND(Box, collect);
    METH_BIND(Box, fetch);
    METH_BIND(Box, gcMark);
    METH_BIND(Box, nextValue);
    METH_BIND(Box, store);
}

// Documented in header.
zvalue TYPE_Box = NULL;

// Documented in header.
zvalue GFN_store = NULL;
