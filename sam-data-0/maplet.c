/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"

#include <stdlib.h>


/*
 * Helper definitions
 */

/** The empty maplet, lazily initialized. */
static zvalue theEmptyMaplet = NULL;


/*
 * API Implementation
 */

/** Documented in API header. */
zmapping samMapletGet(zvalue maplet, zint n) {
    samAssertMaplet(maplet);
    samAssertNth(maplet, n);

    return ((SamMaplet *) maplet)->elems[n];
}

/** Documented in API header. */
zint samMapletFind(zvalue maplet, zvalue key) {
    // TODO: Stuff goes here.
    samDie("TODO");
}

/** Documented in API header. */
zvalue samMapletEmpty(void) {
    if (theEmptyMaplet == NULL) {
	theEmptyMaplet = samAllocValue(SAM_MAPLET, 0, 0);
    }

    return theEmptyMaplet;
}

/** Documented in API header. */
zvalue samMapletPut(zvalue maplet, zvalue key, zvalue value) {
    // TODO: Stuff goes here.
    samDie("TODO");
}
