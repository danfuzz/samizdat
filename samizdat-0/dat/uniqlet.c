/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Private Definitions
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
static UniqletInfo *getInfo(zvalue uniqlet) {
    return pbPayload(uniqlet);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue uniqlet(void) {
    zvalue result = pbAllocValue(TYPE_Uniqlet, sizeof(UniqletInfo));

    getInfo(result)->id = pbOrderId();
    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Uniqlet, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (getInfo(v1)->id < getInfo(v2)->id) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void datBindUniqlet(void) {
    TYPE_Uniqlet = coreTypeFromName(stringFromUtf8(-1, "Uniqlet"));
    METH_BIND(Uniqlet, order);
}

/* Documented in header. */
zvalue TYPE_Uniqlet = NULL;
