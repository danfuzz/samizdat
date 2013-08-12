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
} UniqletInfo;


/**
 * Gets a pointer to the info of a uniqlet.
 */
static UniqletInfo *uniInfo(zvalue uniqlet) {
    return pbPayload(uniqlet);
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertUniqlet(zvalue value) {
    assertHasType(value, TYPE_Uniqlet);
}

/* Documented in header. */
zvalue uniqlet(void) {
    zvalue result = pbAllocValue(TYPE_Uniqlet, sizeof(UniqletInfo));

    uniInfo(result)->id = pbOrderId();
    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Uniqlet_order(zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (uniInfo(v1)->id < uniInfo(v2)->id) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void datBindUniqlet(void) {
    TYPE_Uniqlet = coreTypeFromName(stringFromUtf8(-1, "Uniqlet"));
    gfnBindCore(GFN_order,  TYPE_Uniqlet, Uniqlet_order);
}

/* Documented in header. */
zvalue TYPE_Uniqlet = NULL;
