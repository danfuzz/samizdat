/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions
 */

/** Lowest observed (inclusive) valid heap address. */
static void *heapBottom = NULL;

/** Highest observed (exclusive) valid heap address. */
static void *heapTop = NULL;


/*
 * Exported functions
 */

/* Documented in header. */
void *zalloc(zint size) {
    if (size < 0) {
        die("Invalid allocation size: %lld", size);
    }

    void *result = malloc(size);

    if (result == NULL) {
        die("Failed: malloc(%#llx).", size);
    }

    memset(result, 0, size);

    if ((heapBottom == NULL) || (heapBottom > result)) {
        heapBottom = result;
    }

    void *resultEnd = ((char *) result) + size;
    if (heapTop < resultEnd) {
        heapTop = resultEnd;
    }

    return result;
}

/* Documented in header. */
void zfree(void *memory) {
    free(memory);
}

/* Documented in header. */
bool utilIsHeapAllocated(void *memory) {
    return (memory >= heapBottom) && (memory < heapTop);
}
