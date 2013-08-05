/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** The next "order id" to return. */
static zint nextOrderId = 0;


/*
 * Exported functions
 */

/* Documented in header. */
void pbInit(void) {
    if (GFN_eq != NULL) {
        return;
    }

    pbInitCoreGenerics();
    pbBindDeriv();
    pbBindFunction();
    pbBindGeneric();
}

/* Documented in header. */
zint pbOrderId(void) {
    if (nextOrderId < 0) {
        die("Too many order ids!");
    }

    zint result = nextOrderId;
    nextOrderId++;
    return result;
}
