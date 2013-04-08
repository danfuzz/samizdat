/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"


/*
 * Helper definitions
 */

/** The next uniqlet id to issue. */
static zint theNextId = 0;

/**
 * Gets the id of a uniqlet.
 */
static zint uniqletId(zvalue uniqlet) {
    samAssertUniqlet(uniqlet);

    return ((SamUniqlet *) uniqlet)->id;
}


/*
 * Module functions
 */

/** Documented in `impl.h`. */
zcomparison samUniqletCompare(zvalue v1, zvalue v2) {
    zint id1 = uniqletId(v1);
    zint id2 = uniqletId(v2);

    if (id1 < id2) {
        return SAM_IS_LESS;
    } else if (id1 > id2) {
        return SAM_IS_MORE;
    } else {
        return SAM_IS_EQUAL;
    }
}


/*
 * Exported functions
 */

/** Documented in API header. */
zvalue samUniqlet(void) {
    zvalue result = samAllocValue(SAM_UNIQLET, 0, sizeof(zint));

    if (theNextId < 0) {
        // Shouldn't be possible, but just in case...
        samDie("Shouldn't happen: Way too many uniqlets!");
    }

    ((SamUniqlet *) result)->id = theNextId;
    theNextId++;

    return result;
}
