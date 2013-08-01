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

/* Documented in header. */
bool datInitialized = false;


/*
 * Exported functions
 */

/* Documented in header. */
void datInit(void) {
    zstackPointer save = datFrameStart();

    datInitCoreGenerics();
    datBindDeriv();
    datBindFunction();
    datBindGeneric();
    datBindInt();
    datBindList();
    datBindMap();
    datBindString();
    datBindUniqlet();

    datFrameReturn(save, NULL);

    datInitialized = true;
}

