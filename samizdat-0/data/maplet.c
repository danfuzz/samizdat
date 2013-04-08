/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"

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
 * Module functions
 */

/** Documented in `impl.h`. */
zcomparison samMapletCompare(zvalue v1, zvalue v2) {
    zmapping *e1 = mapletElems(v1);
    zmapping *e2 = mapletElems(v2);
    zint sz1 = samSize(v1);
    zint sz2 = samSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i].key, e2[i].key);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    if (sz1 < sz2) {
        return SAM_IS_LESS;
    } else if (sz1 > sz2) {
        return SAM_IS_MORE;
    }

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i].value, e2[i].value);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    return SAM_IS_EQUAL;
}


/*
 * Exported functions
 */

/** Documented in API header. */
zmapping samMapletGetMapping(zvalue maplet, zint n) {
    samAssertNth(maplet, n);

    return mapletElems(maplet)[n];
}

/** Documented in API header. */
zvalue samMapletGet(zvalue maplet, zvalue key) {
    zint index = samMapletFind(maplet, key);

    return (index < 0) ? NULL : samMapletGetMapping(maplet, index).value;
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
        switch (samCompare(key, elems[guess].key)) {;
            case SAM_IS_LESS: max = guess - 1; break;
            case SAM_IS_MORE: min = guess + 1; break;
            default:          return guess;
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
