/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"


/*
 * Module functions
 */

/* Documented in header. */
zvalue datAllocValue(ztype type, zint size, zint extraBytes) {
    if (size < 0) {
        die("Invalid value size: %lld", size);
    }

    zvalue result = zalloc(sizeof(DatValue) + extraBytes);

    result->magic = DAT_VALUE_MAGIC;
    result->type = type;
    result->size = size;

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
void datImmortalize(zvalue value) {
    // Nothing to do...yet.
}

/* Documented in header. */
void datSetStackBase(void *base) {
    // Nothing to do...yet.
}

/* Documented in header. */
void datGc(void) {
    // Nothing to do...yet.
}
