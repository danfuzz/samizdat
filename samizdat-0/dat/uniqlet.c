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


/**
 * Gets a pointer to the info of a uniqlet.
 */
static DatUniqlet *uniInfo(zvalue uniqlet) {
    return pbPayload(uniqlet);
}

/**
 * Allocates and initializes a uniqlet, without doing error-checking
 * on the arguments.
 */
static zvalue newUniqlet(DatUniqletDispatch *dispatch, void *state) {
    zvalue result = pbAllocValue(DAT_Uniqlet, sizeof(DatUniqlet));

    DatUniqlet *info = uniInfo(result);
    info->id = pbOrderId();
    info->dispatch = dispatch;
    info->state = state;

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue uniqlet(void) {
    return newUniqlet(NULL, NULL);
}

/* Documented in header. */
void *uniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    datAssertUniqlet(uniqlet);

    if (!uniqletHasDispatch(uniqlet, dispatch)) {
        die("Wrong uniqlet dispatch table for get.");
    }

    return uniInfo(uniqlet)->state;
}

/* Documented in header. */
bool uniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch) {
    datAssertUniqlet(uniqlet);
    return (dispatch == uniInfo(uniqlet)->dispatch);
}

/* Documented in header. */
zvalue uniqletFrom(DatUniqletDispatch *dispatch, void *state) {
    return newUniqlet(dispatch, state);
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Uniqlet_gcFree(zvalue state, zint argCount, const zvalue *args) {
    zvalue uniqlet = args[0];
    DatUniqlet *info = uniInfo(uniqlet);

    if (info->dispatch != NULL) {
        info->dispatch->free(info->state);
    }

    return NULL;
}

/* Documented in header. */
static zvalue Uniqlet_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue uniqlet = args[0];
    DatUniqlet *info = uniInfo(uniqlet);

    if (info->dispatch != NULL) {
        info->dispatch->mark(info->state);
    }

    return NULL;
}

/* Documented in header. */
static zvalue Uniqlet_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (uniInfo(v1)->id < uniInfo(v2)->id) ? DAT_NEG1 : DAT_1;
}

/* Documented in header. */
void datBindUniqlet(void) {
    gfnBindCore(GFN_gcFree, DAT_Uniqlet, Uniqlet_gcFree);
    gfnBindCore(GFN_gcMark, DAT_Uniqlet, Uniqlet_gcMark);
    gfnBindCore(GFN_order,  DAT_Uniqlet, Uniqlet_order);
}

/* Documented in header. */
static PbType INFO_Uniqlet = {
    .name = "Uniqlet"
};
ztype DAT_Uniqlet = &INFO_Uniqlet;
