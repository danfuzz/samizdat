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

/** The empty listlet, lazily initialized. */
static zvalue theEmptyListlet = NULL;


/*
 * API Implementation
 */

/** Documented in API header. */
zvalue samListletGet(zvalue listlet, zint n) {
    samAssertListlet(listlet);
    samAssertNth(listlet, n);

    return ((SamListlet *) listlet)->elems[n];
}

/** Documented in API header. */
zvalue samListletEmpty(void) {
    if (theEmptyListlet == NULL) {
	theEmptyListlet = samAllocValue(SAM_LISTLET, 0, 0);
    }

    return theEmptyListlet;
}

/** Documented in API header. */
zvalue samListletAppend(zvalue listlet, zvalue value) {
    // TODO: Stuff goes here.
    samDie("TODO");
}

/** Documented in API header. */
zvalue samListletFromUtf8(const zbyte *string, zint stringSize) {
    // TODO: Stuff goes here.
    samDie("TODO");
}
