/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Box manipulation
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


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
bool boxCanStore(zvalue box) {
    assertHasType(box, TYPE_Box);
    return getInfo(box)->canStore;
}

/* Documented in header. */
zvalue boxGet(zvalue box) {
    assertHasType(box, TYPE_Box);

    zvalue result = getInfo(box)->value;

    if (result != NULL) {
        // The box has a value that we are about to return. Since the box
        // could become garbage after this, we have to treat the value as
        // "escaped" and so explicitly add the result value to the frame at
        // this point. This ensures that GC will be able to find it.
        pbFrameAdd(result);
    }

    return result;
}

/* Documented in header. */
void boxReset(zvalue box) {
    assertHasType(box, TYPE_Box);

    BoxInfo *info = getInfo(box);

    if (info->setOnce) {
        die("Attempt to reset yield box.");
    }

    info->value = NULL;
    info->canStore = true;
}

/* Documented in header. */
void boxSet(zvalue box, zvalue value) {
    assertHasType(box, TYPE_Box);

    if (box == DAT_NULL_BOX) {
        return;
    }

    BoxInfo *info = getInfo(box);

    if (!info->canStore) {
        die("Attempt to re-store yield box.");
    }

    info->value = value;

    if (info->setOnce) {
        info->canStore = false;
    }
}

/* Documented in header. */
zvalue makeMutableBox(void) {
    zvalue result = pbAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->canStore = true;
    info->setOnce = false;

    return result;
}

/* Documented in header. */
zvalue makeYieldBox(void) {
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
zvalue DAT_NULL_BOX = NULL;

/* Documented in header. */
METH_IMPL(Box, gcMark) {
    zvalue box = args[0];
    BoxInfo *info = getInfo(box);

    pbMark(info->value);

    return NULL;
}

/* Documented in header. */
void datBindBox(void) {
    TYPE_Box = coreTypeFromName(stringFromUtf8(-1, "Box"), true);
    METH_BIND(Box, gcMark);

    DAT_NULL_BOX = makeMutableBox(); // Note: Explicit `==` check in `boxSet`.
    pbImmortalize(DAT_NULL_BOX);
}

/* Documented in header. */
zvalue TYPE_Box = NULL;
