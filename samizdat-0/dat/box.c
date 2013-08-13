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

    /** True iff the box is considered to be set (see spec for details). */
    bool isSet;

    /** Uniqlet to use for ordering comparisons. */
    zvalue orderId;
} BoxInfo;

/**
 * Gets a pointer to the value's info.
 */
static BoxInfo *getInfo(zvalue box) {
    return pbPayload(box);
}

/**
 * Gets the order id, initializing it if necessary.
 */
static zvalue boxOrderId(zvalue box) {
    BoxInfo *info = getInfo(box);
    zvalue orderId = info->orderId;

    if (orderId == NULL) {
        orderId = info->orderId = uniqlet();
    }

    return orderId;
}


/*
 * Exported Definitions
 */

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
bool boxIsSet(zvalue box) {
    assertHasType(box, TYPE_Box);
    return getInfo(box)->isSet;
}

/* Documented in header. */
void boxReset(zvalue box) {
    assertHasType(box, TYPE_Box);

    BoxInfo *info = getInfo(box);

    if (info->setOnce) {
        die("Attempt to reset yield box.");
    }

    info->value = NULL;
    info->isSet = false;
}

/* Documented in header. */
void boxSet(zvalue box, zvalue value) {
    assertHasType(box, TYPE_Box);

    if (box == DAT_NULL_BOX) {
        return;
    }

    BoxInfo *info = getInfo(box);

    if (info->isSet && info->setOnce) {
        die("Attempt to re-set yield box.");
    }

    info->value = value;
    info->isSet = true;
}

/* Documented in header. */
zvalue boxMutable(void) {
    zvalue result = pbAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->isSet = false;
    info->setOnce = false;

    return result;
}

/* Documented in header. */
zvalue boxYield(void) {
    zvalue result = pbAllocValue(TYPE_Box, sizeof(BoxInfo));
    BoxInfo *info = getInfo(result);

    info->value = NULL;
    info->isSet = false;
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
    pbMark(info->orderId);

    return NULL;
}

/* Documented in header. */
METH_IMPL(Box, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return intFromZint(pbOrder(boxOrderId(v1), boxOrderId(v2)));
}

/* Documented in header. */
void datBindBox(void) {
    TYPE_Box = coreTypeFromName(stringFromUtf8(-1, "Box"), true);
    METH_BIND(Box, gcMark);
    METH_BIND(Box, order);

    DAT_NULL_BOX = boxMutable(); // Note: Explicit `==` check in `boxSet`.
    pbImmortalize(DAT_NULL_BOX);
}

/* Documented in header. */
zvalue TYPE_Box = NULL;
