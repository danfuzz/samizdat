/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>


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
    samAssertListlet(listlet);

    zint oldSize = listlet->size;
    zint size = oldSize + 1;
    zvalue result = samAllocValue(SAM_LISTLET, size, size * sizeof(zvalue));

    memcpy(((SamListlet *) result)->elems,
           ((SamListlet *) listlet)->elems,
           oldSize * sizeof(zvalue));
    ((SamListlet *) result)->elems[oldSize] = value;

    return result;
}

/** Documented in API header. */
zvalue samListletFromUtf8(const zbyte *string, zint stringSize) {
    // TODO: Stuff goes here.
    samDie("TODO");
}
