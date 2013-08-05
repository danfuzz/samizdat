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
 * Helper definitions
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
} DatBox;

/**
 * Gets a pointer to the value's info.
 */
static DatBox *boxInfo(zvalue box) {
    return pbPayload(box);
}

/**
 * Gets the order id, initializing it if necessary.
 */
static zvalue boxOrderId(zvalue box) {
    DatBox *info = boxInfo(box);
    zvalue orderId = info->orderId;

    if (orderId == NULL) {
        orderId = info->orderId = datUniqlet();
    }

    return orderId;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue boxGet(zvalue box) {
    datAssertBox(box);

    zvalue result = boxInfo(box)->value;

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
    datAssertBox(box);
    return boxInfo(box)->isSet;
}

/* Documented in header. */
void boxReset(zvalue box) {
    datAssertBox(box);

    DatBox *info = boxInfo(box);

    if (info->setOnce) {
        die("Attempt to reset yield box.");
    }

    info->value = NULL;
    info->isSet = false;
}

/* Documented in header. */
void boxSet(zvalue box, zvalue value) {
    datAssertBox(box);

    if (box == DAT_NULL_BOX) {
        return;
    }

    DatBox *info = boxInfo(box);

    if (info->isSet && info->setOnce) {
        die("Attempt to re-set yield box.");
    }

    info->value = value;
    info->isSet = true;
}

/* Documented in header. */
zvalue boxMutable(void) {
    zvalue result = pbAllocValue(DAT_Box, sizeof(DatBox));
    DatBox *info = boxInfo(result);

    info->value = NULL;
    info->isSet = false;
    info->setOnce = false;

    return result;
}

/* Documented in header. */
zvalue boxYield(void) {
    zvalue result = pbAllocValue(DAT_Box, sizeof(DatBox));
    DatBox *info = boxInfo(result);

    info->value = NULL;
    info->isSet = false;
    info->setOnce = true;

    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
zvalue DAT_NULL_BOX = NULL;

/* Documented in header. */
static zvalue Box_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue box = args[0];
    DatBox *info = boxInfo(box);

    pbMark(info->value);
    pbMark(info->orderId);

    return NULL;
}

/* Documented in header. */
static zvalue Box_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return intFromZint(pbOrder(boxOrderId(v1), boxOrderId(v2)));
}

/* Documented in header. */
void datBindBox(void) {
    gfnBindCore(GFN_gcMark, DAT_Box, Box_gcMark);
    gfnBindCore(GFN_order,  DAT_Box, Box_order);

    DAT_NULL_BOX = boxMutable(); // Note: Explicit `==` check in `boxSet`.
    pbImmortalize(DAT_NULL_BOX);
}

/* Documented in header. */
static PbType INFO_Box = {
    .name = "Box"
};
ztype DAT_Box = &INFO_Box;
