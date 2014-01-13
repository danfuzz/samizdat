/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Box manipulation
 */

#include "impl.h"
#include "type/Box.h"
#include "type/Generic.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"


/*
 * Private Definitions
 */

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
    return pbPayload(box);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue makeCell(zvalue value) {
    zvalue result = pbAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = value;
    info->canStore = true;
    info->setOnce = false;

    return result;
}

/* Documented in header. */
zvalue makePromise(void) {
    zvalue result = pbAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->canStore = true;
    info->setOnce = true;

    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
zvalue TYPE_Box = NULL;

/* Documented in header. */
zvalue GFN_canStore;

/* Documented in header. */
zvalue GFN_fetch;

/* Documented in header. */
zvalue GFN_store;

/* Documented in header. */
METH_IMPL(Box, canStore) {
    zvalue box = args[0];
    return getInfo(box)->canStore ? box : NULL;
}

/* Documented in header. */
METH_IMPL(Box, fetch) {
    zvalue box = args[0];
    zvalue result = getInfo(box)->value;

    if (result != NULL) {
        // The box has a value that we are about to return. Since the box
        // could become garbage after this, we have to treat the value as
        // "escaped" and so explicitly add the result value to the frame at
        // this point. This ensures that GC will be able to find it.
        datFrameAdd(result);
    }

    return result;
}

/* Documented in header. */
METH_IMPL(Box, gcMark) {
    zvalue box = args[0];
    BoxInfo *info = getInfo(box);

    pbMark(info->value);

    return NULL;
}

/* Documented in header. */
METH_IMPL(Box, store) {
    zvalue box = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    BoxInfo *info = getInfo(box);

    if (!info->canStore) {
        die("Attempt to re-store yield box.");
    }

    if (info->setOnce) {
        info->canStore = false;
    }

    info->value = value;

    return value;
}

/** Initializes the module. */
MOD_INIT(Box) {
    MOD_USE(Value);

    GFN_canStore = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "canStore"));
    pbImmortalize(GFN_canStore);

    GFN_fetch = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "fetch"));
    pbImmortalize(GFN_fetch);

    GFN_store = makeGeneric(1, 2, GFN_NONE, stringFromUtf8(-1, "store"));
    pbImmortalize(GFN_store);

    TYPE_Box = coreTypeFromName(stringFromUtf8(-1, "Box"), true);

    METH_BIND(Box, canStore);
    METH_BIND(Box, fetch);
    METH_BIND(Box, gcMark);
    METH_BIND(Box, store);
}
