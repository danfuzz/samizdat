/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue uniqlet(void) {
    return pbAllocValue(TYPE_Uniqlet, 0);
}


/*
 * Type Definition
 */

/* Documented in header. */
void datBindUniqlet(void) {
    TYPE_Uniqlet = coreTypeFromName(stringFromUtf8(-1, "Uniqlet"), true);
}

/* Documented in header. */
zvalue TYPE_Uniqlet = NULL;
