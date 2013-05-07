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

/** The next uniqlet id to issue. */
static zint theNextId = 0;

/**
 * Gets a pointer to the info of a uniqlet.
 */
static UniqletInfo *uniqletInfo(zvalue uniqlet) {
    datAssertUniqlet(uniqlet);

    return &((DatUniqlet *) uniqlet)->info;
}

/**
 * Gets the id of a uniqlet.
 */
static zint uniqletId(zvalue uniqlet) {
    return uniqletInfo(uniqlet)->id;
}

/**
 * Allocates and initializes a uniqlet, without doing error-checking
 * on the arguments.
 */
static zvalue newUniqlet(DatUniqletDispatch *dispatch, void *state) {
    zvalue result = datAllocValue(DAT_UNIQLET, 0, sizeof(UniqletInfo));

    if (theNextId < 0) {
        // Shouldn't be possible, but just in case...
        die("Shouldn't happen: Way too many uniqlets!");
    }

    UniqletInfo *info = &((DatUniqlet *) result)->info;
    info->id = theNextId;
    info->dispatch = dispatch;
    info->state = state;
    theNextId++;

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datUniqletOrder(zvalue v1, zvalue v2) {
    zint id1 = uniqletId(v1);
    zint id2 = uniqletId(v2);

    if (id1 < id2) {
        return ZLESS;
    } else if (id1 > id2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}

void datUniqletMark(zvalue value) {
    UniqletInfo *info = uniqletInfo(value);

    if (info->dispatch != NULL) {
        info->dispatch->mark(info->state);
    }
}

void datUniqletFree(zvalue value) {
    UniqletInfo *info = uniqletInfo(value);

    if (info->dispatch != NULL) {
        info->dispatch->free(info->state);
    }
}

/*
 * Exported functions
 */

/* Documented in header. */
zvalue datUniqlet(void) {
    return newUniqlet(NULL, NULL);
}

/* Documented in header. */
zvalue datUniqletWith(DatUniqletDispatch *dispatch, void *state) {
    return newUniqlet(dispatch, state);
}

/* Documented in header. */
bool datUniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    return (dispatch == uniqletInfo(uniqlet)->dispatch);
}

/* Documented in header. */
void *datUniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    if (!datUniqletHasDispatch(uniqlet, dispatch)) {
        die("Wrong uniqlet dispatch table for get.");
    }

    return uniqletInfo(uniqlet)->state;
}

