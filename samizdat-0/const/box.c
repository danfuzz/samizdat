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
 * Box state. Instances of this structure are bound as the closure state
 * as part of function registration in the implementation of the box
 * constructor primitives.
 */
typedef struct {
    /** Content value. */
    zvalue value;

    /** True iff this is a set-once (yield) box. */
    bool setOnce;

    /** True iff the box is considered to be set (see spec for details). */
    bool isSet;
} Box;

/**
 * Marks a box state for garbage collection.
 */
static void boxMark(void *state) {
    datMark(((Box *) state)->value);
}

/**
 * Frees an object state.
 */
static void boxFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for boxes. */
static DatUniqletDispatch BOX_DISPATCH = {
    boxMark,
    boxFree
};

/**
 * Returns `true` iff this is the special "null box" value.
 */
static bool isNullBox(zvalue value) {
    return datEq(value, TOK_CAP_NULL_BOX);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue boxGet(zvalue boxUniqlet) {
    if (isNullBox(boxUniqlet)) {
        return NULL;
    }

    Box *box = datUniqletGetState(boxUniqlet, &BOX_DISPATCH);
    zvalue result = box->value;

    if ((result != NULL) && !box->setOnce) {
        // The box is mutable, and we are about to return a non-empty result.
        // The returned value could conceivably be "detached" from the box
        // (if the box is reset or re-set), so we have to explicitly add the
        // result value to the frame at this point. This ensures that GC will
        // be able to find it.
        datFrameAdd(result);
    }

    return result;
}

/* Documented in header. */
bool boxIsSet(zvalue boxUniqlet) {
    if (isNullBox(boxUniqlet)) {
        return false;
    }

    Box *box = datUniqletGetState(boxUniqlet, &BOX_DISPATCH);
    return box->isSet;
}

/* Documented in header. */
void boxReset(zvalue boxUniqlet) {
    if (isNullBox(boxUniqlet)) {
        return;
    }

    Box *box = datUniqletGetState(boxUniqlet, &BOX_DISPATCH);

    if (box->setOnce) {
        die("Attempt to reset yield box.");
    }

    box->value = NULL;
    box->isSet = false;
}

/* Documented in header. */
void boxSet(zvalue boxUniqlet, zvalue value) {
    if (isNullBox(boxUniqlet)) {
        return;
    }

    Box *box = datUniqletGetState(boxUniqlet, &BOX_DISPATCH);

    if (box->isSet && box->setOnce) {
        die("Attempt to re-set yield box.");
    }

    box->value = value;
    box->isSet = true;
}

/* Documented in header. */
zvalue boxMutable(void) {
    Box *box = utilAlloc(sizeof(Box));

    box->value = NULL;
    box->isSet = false;
    box->setOnce = false;

    return datUniqletWith(&BOX_DISPATCH, box);
}

/* Documented in header. */
zvalue boxYield(void) {
    Box *box = utilAlloc(sizeof(Box));

    box->value = NULL;
    box->isSet = false;
    box->setOnce = true;

    return datUniqletWith(&BOX_DISPATCH, box);
}
