/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Uniqlet.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue makeUniqlet(void) {
    return pbAllocValue(TYPE_Uniqlet, 0);
}


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindUniqlet(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Uniqlet`.

    // This function is just left here as a positive indication that there
    // is nothing to do.
}

/* Documented in header. */
zvalue TYPE_Uniqlet = NULL;
