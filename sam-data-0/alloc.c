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

/** Documented in `alloc.h`. */
void *samAlloc(zint size) {
    void *result = malloc(size);

    if (result == NULL) {
        samDie("Failed: malloc(%#llx).", size);
    }

    memset(result, 0, size);
    return result;
}

/** Documented in `alloc.h`. */
zvalue samAllocValue(ztype type, zint size, zint extraBytes) {
    zvalue result = samAlloc(sizeof(SamValue) + extraBytes);

    result->magic = SAM_VALUE_MAGIC;
    result->type = type;
    result->size = size;

    return result;
}
