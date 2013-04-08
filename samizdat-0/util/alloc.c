/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "util.h"

#include <stdlib.h>
#include <string.h>


/*
 * Exported functions
 */

/* Documented in header. */
void *samAlloc(zint size) {
    if (size < 0) {
        samDie("Invalid allocation size: %lld", size);
    }

    void *result = malloc(size);

    if (result == NULL) {
        samDie("Failed: malloc(%#llx).", size);
    }

    memset(result, 0, size);
    return result;
}
