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

/**
 * Given a maplet, find the index of the given key. `maplet` must be a
 * maplet. Returns the index of the key or `~insertionIndex` (a
 * negative number) if not found.
 */
static zint mapletFind(zvalue maplet, zvalue key) {
    datAssertMaplet(maplet);
    datAssertValid(key);

    zmapping *elems = mapletElems(maplet);
    zint min = 0;
    zint max = maplet->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (datOrder(key, elems[guess].key)) {;
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


/*
 * Module functions
 */

/* Documented in header. */
zorder datMapletCompare(zvalue v1, zvalue v2) {
    zmapping *e1 = mapletElems(v1);
    zmapping *e2 = mapletElems(v2);
    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zorder result = datOrder(e1[i].key, e2[i].key);
        if (result != ZSAME) {
            return result;
        }
    }

    if (sz1 < sz2) {
        return ZLESS;
    } else if (sz1 > sz2) {
        return ZMORE;
    }

    for (zint i = 0; i < sz; i++) {
        zorder result = datOrder(e1[i].value, e2[i].value);
        if (result != ZSAME) {
            return result;
        }
    }

    return ZSAME;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datMapletEmpty(void) {
    if (theEmptyMaplet == NULL) {
        theEmptyMaplet = allocMaplet(0);
    }

    return theEmptyMaplet;
}

/* Documented in header. */
zvalue datMapletKeys(zvalue maplet) {
    datAssertMaplet(maplet);

    zint size = datSize(maplet);
    zmapping *elems = mapletElems(maplet);
    zvalue result = datListletEmpty();

    for (zint i = 0; i < size; i++) {
        result = datListletAppend(result, elems[i].key);
    }

    return result;
}

/* Documented in header. */
zvalue datMapletGet(zvalue maplet, zvalue key) {
    zint index = mapletFind(maplet, key);

    return (index < 0) ? NULL : mapletElems(maplet)[index].value;
}

/* Documented in header. */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value) {
    zint index = mapletFind(maplet, key);
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

/* Documented in header. */
zvalue datMapletDelete(zvalue maplet, zvalue key) {
    zint index = mapletFind(maplet, key);

    if (index < 0) {
        die("Can't delete nonexistent maplet key.");
    }

    zint size = datSize(maplet) - 1;
    zvalue result = allocMaplet(size);
    zmapping *elems = mapletElems(result);
    zmapping *oldElems = mapletElems(maplet);

    memcpy(elems, oldElems, index * sizeof(zmapping));
    memcpy(elems + index, oldElems + index + 1,
           (size - index) * sizeof(zmapping));
    return result;
}
