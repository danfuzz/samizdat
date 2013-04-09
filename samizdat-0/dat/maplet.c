/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

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
    return datAllocValue(DAT_MAPLET, size, size * sizeof(zmapping));
}

/**
 * Gets the elements array from a maplet.
 */
static zmapping *mapletElems(zvalue maplet) {
    datAssertMaplet(maplet);

    return ((DatMaplet *) maplet)->elems;
}


/*
 * Module functions
 */

/* Documented in header. */
zcomparison datMapletCompare(zvalue v1, zvalue v2) {
    zmapping *e1 = mapletElems(v1);
    zmapping *e2 = mapletElems(v2);
    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = datCompare(e1[i].key, e2[i].key);
        if (result != ZEQUAL) {
            return result;
        }
    }

    if (sz1 < sz2) {
        return ZLESS;
    } else if (sz1 > sz2) {
        return ZMORE;
    }

    for (zint i = 0; i < sz; i++) {
        zcomparison result = datCompare(e1[i].value, e2[i].value);
        if (result != ZEQUAL) {
            return result;
        }
    }

    return ZEQUAL;
}


/*
 * Exported functions
 */

/* Documented in header. */
zmapping datMapletGetMapping(zvalue maplet, zint n) {
    datAssertNth(maplet, n);

    return mapletElems(maplet)[n];
}

/* Documented in header. */
zvalue datMapletGet(zvalue maplet, zvalue key) {
    zint index = datMapletFind(maplet, key);

    return (index < 0) ? NULL : datMapletGetMapping(maplet, index).value;
}

/* Documented in header. */
zint datMapletFind(zvalue maplet, zvalue key) {
    datAssertValid(key);
    datAssertMaplet(maplet);

    zmapping *elems = mapletElems(maplet);
    zint min = 0;
    zint max = maplet->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (datCompare(key, elems[guess].key)) {;
            case ZLESS: max = guess - 1; break;
            case ZMORE: min = guess + 1; break;
            default:    return guess;
        }
    }

    // Not found. The insert point is at `min`. Per the API,
    // this is represented as `~min` (and not, in particular, as `-max`)
    // so that an insertion point of `0` can be unambiguously
    // represented.

    return ~min;
}

/* Documented in header. */
zvalue datMapletEmpty(void) {
    if (theEmptyMaplet == NULL) {
        theEmptyMaplet = allocMaplet(0);
    }

    return theEmptyMaplet;
}

/* Documented in header. */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value) {
    zint index = datMapletFind(maplet, key);
    zint size = datSize(maplet);
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
