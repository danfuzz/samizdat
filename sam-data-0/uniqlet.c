/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"

/*
 * Helper definitions
 */

/** The next uniqlet id to issue. */
static zint theNextId = 0;


/*
 * API implementation
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
