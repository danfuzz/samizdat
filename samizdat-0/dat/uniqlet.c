/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Uniqlet structure.
 */
typedef struct {
    /** Uniqlet unique id. */
    zint id;

    /** Dispatch table. */
    DatUniqletDispatch *dispatch;

    /** Sealed box payload value. */
    void *state;
} DatUniqlet;

/** The next uniqlet id to issue. */
static zint theNextId = 0;

/**
 * Gets a pointer to the info of a uniqlet.
 */
static DatUniqlet *uniqletInfo(zvalue uniqlet) {
    return datPayload(uniqlet);
}

/**
 * Allocates and initializes a uniqlet, without doing error-checking
 * on the arguments.
 */
static zvalue newUniqlet(DatUniqletDispatch *dispatch, void *state) {
    zvalue result = datAllocValue(DAT_Uniqlet, sizeof(DatUniqlet));

    if (theNextId < 0) {
        // Shouldn't be possible, but just in case...
        die("Shouldn't happen: Way too many uniqlets!");
    }

    DatUniqlet *info = uniqletInfo(result);
    info->id = theNextId;
    info->dispatch = dispatch;
    info->state = state;
    theNextId++;

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datUniqlet(void) {
    return newUniqlet(NULL, NULL);
}

/* Documented in header. */
void *datUniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    datAssertUniqlet(uniqlet);

    if (!datUniqletHasDispatch(uniqlet, dispatch)) {
        die("Wrong uniqlet dispatch table for get.");
    }

    return uniqletInfo(uniqlet)->state;
}

/* Documented in header. */
bool datUniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    datAssertUniqlet(uniqlet);
    return (dispatch == uniqletInfo(uniqlet)->dispatch);
}

/* Documented in header. */
zvalue datUniqletWith(DatUniqletDispatch *dispatch, void *state) {
    return newUniqlet(dispatch, state);
}


/*
 * Type binding
 */

/* Documented in header. */
static void uniqletGcMark(zvalue uniqlet) {
    DatUniqlet *info = uniqletInfo(uniqlet);

    if (info->dispatch != NULL) {
        info->dispatch->mark(info->state);
    }
}

/* Documented in header. */
static void uniqletGcFree(zvalue uniqlet) {
    DatUniqlet *info = uniqletInfo(uniqlet);

    if (info->dispatch != NULL) {
        info->dispatch->free(info->state);
    }
}

/* Documented in header. */
static zorder uniqletOrder(zvalue v1, zvalue v2) {
    zint id1 = uniqletInfo(v1)->id;
    zint id2 = uniqletInfo(v2)->id;

    if (id1 < id2) {
        return ZLESS;
    } else if (id1 > id2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}

/* Documented in header. */
void datBindUniqlet(void) {
    // Nothing to do here...yet.
}

/* Documented in header. */
static DatType INFO_Uniqlet = {
    .name = "Uniqlet",
    .typeOf = NULL,
    .gcMark = uniqletGcMark,
    .gcFree = uniqletGcFree,
    .eq = NULL,
    .order = uniqletOrder
};
ztype DAT_Uniqlet = &INFO_Uniqlet;
