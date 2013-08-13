/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stdlib.h>
#include <string.h>


/*
 * Private Definitions
 */

/**
 * Map structure.
 */
typedef struct {
    /** Number of mappings. */
    zint size;

    /** List of mappings, in key-sorted order. */
    zmapping elems[/*size*/];
} MapInfo;

/**
 * Allocates a map of the given size.
 */
static zvalue allocMap(zint size) {
    zvalue result =
        pbAllocValue(TYPE_Map, sizeof(MapInfo) + size * sizeof(zmapping));

    ((MapInfo *) pbPayload(result))->size = size;
    return result;
}

/**
 * Gets the size of a map.
 */
static zint mapSizeOf(zvalue map) {
    return ((MapInfo *) pbPayload(map))->size;
}

/**
 * Gets the elements array from a map.
 */
static zmapping *mapElems(zvalue map) {
    return ((MapInfo *) pbPayload(map))->elems;
}

/**
 * Allocates and returns a map with up to two mappings. This will
 * return a single-mapping map if the two keys are the same, in which case
 * the *second* value is used.
 */
static zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    zorder comp = pbOrder(k1, k2);

    if (comp == ZSAME) {
        return mapping(k2, v2);
    }

    zvalue result = allocMap(2);
    zmapping *elem0 = mapElems(result);
    zmapping *elem1 = &elem0[1];

    if (comp == ZLESS) {
        elem0->key = k1;
        elem0->value = v1;
        elem1->key = k2;
        elem1->value = v2;
    } else {
        elem0->key = k2;
        elem0->value = v2;
        elem1->key = k1;
        elem1->value = v1;
    }

    return result;
}

/**
 * Given a map, find the index of the given key. `map` must be a map.
 * Returns the index of the key if found. If not found, then this returns
 * `~insertionIndex` (a negative number).
 */
static zint mapFind(zvalue map, zvalue key) {
    MapCacheEntry *entry = mapGetCacheEntry(map, key);

    if ((entry->map == map) && (entry->key == key)) {
        return entry->index;
    }

    // Note: There's no need to do assertions on the cache hit path, since
    // we wouldn't have found an invalid entry.

    datAssertMap(map);
    pbAssertValid(key);

    entry->map = map;
    entry->key = key;

    zmapping *elems = mapElems(map);
    zint min = 0;
    zint max = mapSizeOf(map) - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (pbOrder(key, elems[guess].key)) {
            case ZLESS: max = guess - 1; break;
            case ZMORE: min = guess + 1; break;
            default: {
                entry->index = guess;
                return guess;
            }
        }
    }

    // Not found. The insert point is at `min`. Per the API, this is
    // represented as `~index` (and not, in particular, as `-index`)
    // so that an insertion point of `0` can be unambiguously
    // represented.

    zint result = entry->index = ~min;
    return result;
}

/**
 * Mapping comparison function, passed to standard library sorting
 * functions.
 */
static int mappingOrder(const void *m1, const void *m2) {
    return pbOrder(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void datAssertMap(zvalue value) {
    assertHasType(value, TYPE_Map);
}

/* Documented in header. */
void datAssertMapSize1(zvalue value) {
    datAssertMap(value);
    if (pbSize(value) != 1) {
        die("Not a size 1 map.");
    }
}

/* Documented in header. */
void arrayFromMap(zmapping *result, zvalue map) {
    datAssertMap(map);
    memcpy(result, mapElems(map), mapSizeOf(map) * sizeof(zmapping));
}

/* Documented in header. */
zvalue mapAdd(zvalue map1, zvalue map2) {
    datAssertMap(map1);
    datAssertMap(map2);

    zint size1 = mapSizeOf(map1);
    zint size2 = mapSizeOf(map2);

    if (size1 == 0) {
        return map2;
    } else if (size2 == 0) {
        return map1;
    }

    return mapAddArray(map1, size2, mapElems(map2));
}

/* Documented in header. */
zvalue mapAddArray(zvalue map, zint size, const zmapping *mappings) {
    datAssertMap(map);

    if (size == 0) {
        return map;
    } else if (size == 1) {
        return mapPut(map, mappings[0].key, mappings[0].value);
    }

    zint mapSize = mapSizeOf(map);
    zint resultSize = mapSize + size;
    zvalue result = allocMap(resultSize);
    zmapping *elems = mapElems(result);

    // Add all the mappings to the result, and sort it using mergesort.
    // Mergesort is stable and operates best on sorted data, and as it
    // happens the starting map is guaranteed to be sorted.

    memcpy(elems, mapElems(map), mapSize * sizeof(zmapping));
    memcpy(&elems[mapSize], mappings, size * sizeof(zmapping));
    mergesort(elems, resultSize, sizeof(zmapping), mappingOrder);

    // Remove all but the last of any sequence of equal-keys mappings.
    // The last one is preferred, since by construction that's the last
    // of any equal keys from the newly-added mappings.

    zint at = 1;
    for (zint i = 1; i < resultSize; i++) {
        if (pbEq(elems[i].key, elems[at-1].key)) {
            at--;
        }

        if (at != i) {
            elems[at] = elems[i];
        }

        at++;
    }

    ((MapInfo *) pbPayload(result))->size = at;
    return result;
}

/* Documented in header. */
zvalue mapDel(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    if (index < 0) {
        return map;
    }

    zint size = mapSizeOf(map) - 1;

    if (size == 0) {
        return EMPTY_MAP;
    }

    zvalue result = allocMap(size);
    zmapping *elems = mapElems(result);
    zmapping *oldElems = mapElems(map);

    memcpy(elems, oldElems, index * sizeof(zmapping));
    memcpy(&elems[index], &oldElems[index + 1],
           (size - index) * sizeof(zmapping));
    return result;
}

/* Documented in header. */
zvalue mapGet(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    return (index < 0) ? NULL : mapElems(map)[index].value;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mapNth(zvalue map, zint n) {
    datAssertMap(map);

    if ((n < 0) || (n >= mapSizeOf(map))) {
        return NULL;
    }

    if (mapSizeOf(map) == 1) {
        return map;
    }

    zmapping *m = &mapElems(map)[n];
    return mapping(m->key, m->value);
}

/* Documented in header. */
zvalue mapPut(zvalue map, zvalue key, zvalue value) {
    datAssertMap(map);
    pbAssertValid(value);

    zint size = mapSizeOf(map);

    switch (size) {
        case 0: {
            pbAssertValid(key);
            return mapping(key, value);
        }
        case 1: {
            pbAssertValid(key);
            zmapping *elems = mapElems(map);
            return mapFrom2(elems[0].key, elems[0].value, key, value);
        }
    }

    zint index = mapFind(map, key);
    zvalue result;

    if (index >= 0) {
        // The key exists in the given map, so we need to perform
        // a replacement.
        result = allocMap(size);
        memcpy(mapElems(result), mapElems(map), size * sizeof(zmapping));
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = allocMap(size + 1);

        zmapping *origElems = mapElems(map);
        zmapping *resultElems = mapElems(result);

        memcpy(resultElems, origElems, index * sizeof(zmapping));
        memcpy(&resultElems[index + 1], &origElems[index],
               (size - index) * sizeof(zmapping));
    }

    zmapping *elem = &mapElems(result)[index];
    elem->key = key;
    elem->value = value;
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mapping(zvalue key, zvalue value) {
    zvalue result = allocMap(1);
    zmapping *elems = mapElems(result);

    elems->key = key;
    elems->value = value;
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mappingKey(zvalue map) {
    datAssertMapSize1(map);
    return mapElems(map)[0].key;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mappingValue(zvalue map) {
    datAssertMapSize1(map);
    return mapElems(map)[0].value;
}


/*
 * Type Definition
 */

/* Documented in header. */
zvalue EMPTY_MAP = NULL;

/* Documented in header. */
METH_IMPL(Map, eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zint sz1 = mapSizeOf(v1);
    zint sz2 = mapSizeOf(v2);

    if (sz1 != sz2) {
        return NULL;
    }

    zmapping *elems1 = mapElems(v1);
    zmapping *elems2 = mapElems(v2);

    for (zint i = 0; i < sz1; i++) {
        zmapping *e1 = &elems1[i];
        zmapping *e2 = &elems2[i];
        if (!(pbEq(e1->key, e2->key) && pbEq(e1->value, e2->value))) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
METH_IMPL(Map, gcMark) {
    zvalue map = args[0];
    zint size = mapSizeOf(map);
    zmapping *elems = mapElems(map);

    for (zint i = 0; i < size; i++) {
        pbMark(elems[i].key);
        pbMark(elems[i].value);
    }

    return NULL;
}

/* Documented in header. */
METH_IMPL(Map, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zmapping *e1 = mapElems(v1);
    zmapping *e2 = mapElems(v2);
    zint sz1 = mapSizeOf(v1);
    zint sz2 = mapSizeOf(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zorder result = pbOrder(e1[i].key, e2[i].key);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    if (sz1 < sz2) {
        return PB_NEG1;
    } else if (sz1 > sz2) {
        return PB_1;
    }

    for (zint i = 0; i < sz; i++) {
        zorder result = pbOrder(e1[i].value, e2[i].value);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    return PB_0;
}

/* Documented in header. */
METH_IMPL(Map, size) {
    zvalue map = args[0];
    return intFromZint(mapSizeOf(map));
}

/* Documented in header. */
void datBindMap(void) {
    TYPE_Map = coreTypeFromName(stringFromUtf8(-1, "Map"));
    METH_BIND(Map, eq);
    METH_BIND(Map, gcMark);
    METH_BIND(Map, order);
    METH_BIND(Map, size);

    EMPTY_MAP = allocMap(0);
    pbImmortalize(EMPTY_MAP);
}

/* Documented in header. */
zvalue TYPE_Map = NULL;
