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

/** The empty maplet, lazily initialized. */
static zvalue theEmptyMaplet = NULL;

/**
 * Allocates a maplet of the given size.
 */
static zvalue allocMaplet(zint size) {
    return samAllocValue(SAM_MAPLET, size, size * sizeof(zmapping));
}

/**
 * Gets the elements array from a maplet.
 */
static zmapping *mapletElems(zvalue maplet) {
    samAssertMaplet(maplet);

    return ((SamMaplet *) maplet)->elems;
}


/*
 * API Implementation
 */

/** Documented in API header. */
zmapping samMapletGet(zvalue maplet, zint n) {
    samAssertNth(maplet, n);

    return mapletElems(maplet)[n];
}

/** Documented in API header. */
zint samMapletFind(zvalue maplet, zvalue key) {
    samAssertValid(key);
    samAssertMaplet(maplet);

    zmapping *elems = mapletElems(maplet);
    zint min = 0;
    zint max = maplet->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        zcomparison c = samCompare(key, elems[guess].key);
        if (c == SAM_IS_LESS) {
            max = guess - 1;
        } else if (c == SAM_IS_MORE) {
            min = guess + 1;
        } else {
            return guess;
        }
    }

    // Not found. The insert point is at `min`. Per the API,
    // this is represented as `~min` (and not, in particular, as `-max`)
    // so that an insertion point of `0` can be unambiguously
    // represented.

    return ~min;
}

/** Documented in API header. */
zvalue samMapletEmpty(void) {
    if (theEmptyMaplet == NULL) {
        theEmptyMaplet = allocMaplet(0);
    }

    return theEmptyMaplet;
}

/** Documented in API header. */
zvalue samMapletPut(zvalue maplet, zvalue key, zvalue value) {
    zint index = samMapletFind(maplet, key);
    zint size = samSize(maplet);
    zvalue result;

    if (index >= 0) {
        // The key exists in the given maplet, so we need to perform
        // a replacement.
        result = allocMaplet(size);
        memcpy(mapletElems(result), mapletElems(maplet),
               size * sizeof(zmapping));
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = allocMaplet(size + 1);
        memcpy(mapletElems(result), mapletElems(maplet),
               index * sizeof(zmapping));
        memcpy(mapletElems(result) + index + 1,
               mapletElems(maplet) + index,
               (size - index) * sizeof(zmapping));
    }

    mapletElems(result)[index].key = key;
    mapletElems(result)[index].value = value;
    return result;
}
