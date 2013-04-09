/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

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
    datAssertUniqlet(uniqlet);

    return ((SamUniqlet *) uniqlet)->id;
}


/*
 * Module functions
 */

/* Documented in header. */
zcomparison datUniqletCompare(zvalue v1, zvalue v2) {
    zint id1 = uniqletId(v1);
    zint id2 = uniqletId(v2);

    if (id1 < id2) {
        return ZLESS;
    } else if (id1 > id2) {
        return ZMORE;
    } else {
        return ZEQUAL;
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datUniqlet(void) {
    zvalue result = datAllocValue(SAM_UNIQLET, 0, sizeof(zint));

    if (theNextId < 0) {
        // Shouldn't be possible, but just in case...
        die("Shouldn't happen: Way too many uniqlets!");
    }

    ((SamUniqlet *) result)->id = theNextId;
    theNextId++;

    return result;
}
