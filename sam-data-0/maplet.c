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


/*
 * API Implementation
 */

/** Documented in API header. */
zmapping samMapletGet(zvalue maplet, zint n) {
    samAssertMaplet(maplet);
    samAssertNth(maplet, n);

    return ((SamMaplet *) maplet)->elems[n];
}

/** Documented in API header. */
zint samMapletFind(zvalue maplet, zvalue key) {
    samAssertValid(key);
    samAssertMaplet(maplet);

    zmapping *elems = ((SamMaplet *) maplet)->elems;
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
        theEmptyMaplet = samAllocValue(SAM_MAPLET, 0, 0);
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
        result = samAllocValue(SAM_MAPLET, size, sizeof(zmapping));
        memcpy(((SamMaplet *) result)->elems,
               ((SamMaplet *) maplet)->elems,
               size * sizeof(zmapping));
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = samAllocValue(SAM_MAPLET, size + 1, sizeof(zmapping));
        memcpy(((SamMaplet *) result)->elems,
               ((SamMaplet *) maplet)->elems,
               index * sizeof(zmapping));
        memcpy(((SamMaplet *) result)->elems + index + 1,
               ((SamMaplet *) maplet)->elems + index,
               (size - index) * sizeof(zmapping));
    }

    ((SamMaplet *) result)->elems[index].key = key;        
    ((SamMaplet *) result)->elems[index].value = value;
    return result;
}
