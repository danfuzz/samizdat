/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions
 */

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
    return ((DatMaplet *) maplet)->elems;
}

/**
 * Allocates and returns a maplet with the single given mapping.
 */
static zvalue mapletFrom1(zvalue key, zvalue value) {
    zvalue result = allocMaplet(1);
    zmapping *elems = mapletElems(result);

    elems->key = key;
    elems->value = value;
    return result;
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

/**
 * Mapping comparison function, passed to standard library sorting
 * functions.
 */
static int mappingOrder(const void *m1, const void *m2) {
    return datOrder(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datMapletOrder(zvalue v1, zvalue v2) {
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
    return allocMaplet(0);
}

/* Documented in header. */
zvalue datMapletGet(zvalue maplet, zvalue key) {
    zint index = mapletFind(maplet, key);

    return (index < 0) ? NULL : mapletElems(maplet)[index].value;
}

/* Documented in header. */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value) {
    datAssertValid(value);

    zint index = mapletFind(maplet, key);
    zint size = datSize(maplet);

    if (size == 0) {
        return mapletFrom1(key, value);
    }

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

/* TODO: Documented in header. */
zvalue datMapletPutArrays(zvalue maplet, zint size,
                          const zvalue *keys, const zvalue *values) {
    datAssertMaplet(maplet);

    if (size == 1) {
        return datMapletPut(maplet, keys[0], values[0]);
    }

    zint mapletSize = datSize(maplet);
    zint resultSize = mapletSize + size;
    zvalue result = allocMaplet(resultSize);
    zmapping *elems = mapletElems(result);

    // Add the new mappings to the result, and sort it.

    for (zint i = 0; i < size; i++) {
        elems[i].key = keys[i];
        elems[i].value = values[i];
    }

    qsort(elems, size, sizeof(zmapping), mappingOrder);

    // Add the original maplet mappings, and then mergesort them.
    // Mergesort is stable and operates best on sorted data, which
    // is why we bother sorting the new mappings above.

    memcpy(&elems[size], mapletElems(maplet), mapletSize * sizeof(zmapping));
    mergesort(elems, resultSize, sizeof(zmapping), mappingOrder);

    // Remove one of any pair of equal-keys mappings, preferring the earlier
    // one, since by construction that's from the newly-added mappings.

    zint at = 0;
    for (zint i = 0; i < resultSize; i++) {
        if (at != i) {
            elems[at] = elems[i];
        }
        at++;

        if (i == (resultSize - 1)) {
            break;
        }

        if (datOrder(elems[i].key, elems[i+1].key) == ZSAME) {
            i++;
        }
    }

    result->size = at;
    return result;
}

/* Documented in header. */
zvalue datMapletAdd(zvalue maplet1, zvalue maplet2) {
    datAssertMaplet(maplet1);
    datAssertMaplet(maplet2);

    zint size1 = datSize(maplet1);
    zint size2 = datSize(maplet2);

    if (size1 == 0) {
        return maplet2;
    } else if (size2 == 0) {
        return maplet1;
    }

    zvalue result = maplet1;
    zmapping *elems = mapletElems(maplet2);

    for (zint i = 0; i < size2; i++) {
        result = datMapletPut(result, elems[i].key, elems[i].value);
    }

    return result;
}

/* Documented in header. */
zvalue datMapletDel(zvalue maplet, zvalue key) {
    zint index = mapletFind(maplet, key);

    if (index < 0) {
        return maplet;
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

/* Documented in Samizdat Layer 0 spec. */
zvalue datMapletNth(zvalue maplet, zint n) {
    datAssertMaplet(maplet);

    if (!datHasNth(maplet, n)) {
        return NULL;
    }

    if (datSize(maplet) == 1) {
        return maplet;
    }

    zmapping *mapping = &mapletElems(maplet)[n];
    return mapletFrom1(mapping->key, mapping->value);
}

/* Documented in Samizdat Layer 0 spec. */
zvalue datMapletNthKey(zvalue maplet, zint n) {
    datAssertMaplet(maplet);
    datAssertNth(maplet, n);

    if (!datHasNth(maplet, n)) {
        return NULL;
    }

    return mapletElems(maplet)[n].key;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue datMapletNthValue(zvalue maplet, zint n) {
    datAssertMaplet(maplet);
    datAssertNth(maplet, n);

    if (!datHasNth(maplet, n)) {
        return NULL;
    }

    return mapletElems(maplet)[n].value;
}
