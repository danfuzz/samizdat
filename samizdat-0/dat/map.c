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
 * Helper definitions
 */

enum {
    /** Whether to spew to the console about map cache hits. */
    CHATTY_CACHEY = false
};

/**
 * Entry in the map cache. The cache is used to speed up calls to `mapFind`.
 * In practice it looks like the theoretical best case is probably about
 * 74% (that is, nearly 3 of 4 lookups are for a map/key pair that have
 * been observed before). The size of the map cache is chosen to hit the
 * point of diminishing returns.
 */
typedef struct {
    /** Map to look up a key in. */
    zvalue map;

    /** Key to look up. */
    zvalue key;

    /** Result from `mapFind` (see which for details). */
    zint index;
} CacheEntry;

/** The cache of `mapFind` lookups. */
static CacheEntry mapCache[DAT_MAP_CACHE_SIZE];

/**
 * Gets the `CacheEntry` for the given map/key pair.
 */
static CacheEntry *getCacheEntry(zvalue map, zvalue key) {
    uintptr_t hash = ((uintptr_t) map >> 4) + (((uintptr_t) key) >> 4) * 31;
    hash ^= (hash >> 16) ^ (hash >> 32) ^ (hash >> 48);

    // Note: In practice there doesn't seem to be an observable difference
    // between using `&` and `%` to calculate the cache index, so we go
    // for `%` and a prime number cache size to get probably-better cache
    // behavior.
    CacheEntry *entry = &mapCache[hash % DAT_MAP_CACHE_SIZE];

    if (CHATTY_CACHEY) {
        static int hits = 0;
        static int total = 0;
        if ((entry->map == map) && (entry->key == key)) {
            hits++;
        }
        total++;
        if ((total % 10000000) == 0) {
            note("Map Cache: Hit rate %d/%d == %5.2f%%", hits, total,
                (100.0 * hits) / total);
        }
    }

    return entry;
}

/**
 * Allocates a map of the given size.
 */
static zvalue allocMap(zint size) {
    zvalue result =
        datAllocValue(DAT_Map, sizeof(DatMap) + size * sizeof(zmapping));

    ((DatMap *) result)->size = size;
    return result;
}

/**
 * Gets the size of a map.
 */
static zint mapSizeOf(zvalue map) {
    return ((DatMap *) map)->size;
}

/**
 * Gets the elements array from a map.
 */
static zmapping *mapElems(zvalue map) {
    return ((DatMap *) map)->elems;
}

/**
 * Allocates and returns a map with the single given mapping.
 */
static zvalue mapFrom1(zvalue key, zvalue value) {
    zvalue result = allocMap(1);
    zmapping *elems = mapElems(result);

    elems->key = key;
    elems->value = value;
    return result;
}

/**
 * Allocates and returns a map with up to two mappings. This will
 * return a single-mapping map if the two keys are the same, in which case
 * the *second* value is used.
 */
static zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    zorder comp = datOrder(k1, k2);

    if (comp == ZSAME) {
        return mapFrom1(k2, v2);
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
    CacheEntry *entry = getCacheEntry(map, key);

    if ((entry->map == map) && (entry->key == key)) {
        return entry->index;
    }

    // Note: There's no need to do assertions on the cache hit path, since
    // we wouldn't have found an invalid entry.

    datAssertMap(map);
    datAssertValid(key);

    entry->map = map;
    entry->key = key;

    zmapping *elems = mapElems(map);
    zint min = 0;
    zint max = mapSizeOf(map) - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (datOrder(key, elems[guess].key)) {
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
    return datOrder(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}


/*
 * Module functions
 */

/* Documented in header. */
void datMapClearCache(void) {
    memset(mapCache, 0, sizeof(mapCache));
}


/*
 * Exported functions
 */

/* Documented in header. */
void datArrayFromMap(zmapping *result, zvalue map) {
    datAssertMap(map);
    memcpy(result, mapElems(map), mapSizeOf(map) * sizeof(zmapping));
}

/* Documented in header. */
zvalue datMapAdd(zvalue map1, zvalue map2) {
    datAssertMap(map1);
    datAssertMap(map2);

    zint size1 = mapSizeOf(map1);
    zint size2 = mapSizeOf(map2);

    if (size1 == 0) {
        return map2;
    } else if (size2 == 0) {
        return map1;
    }

    return datMapAddArray(map1, size2, mapElems(map2));
}

/* Documented in header. */
zvalue datMapAddArray(zvalue map, zint size, const zmapping *mappings) {
    datAssertMap(map);

    if (size == 0) {
        return map;
    } else if (size == 1) {
        return datMapPut(map, mappings[0].key, mappings[0].value);
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
        if (datEq(elems[i].key, elems[at-1].key)) {
            at--;
        }

        if (at != i) {
            elems[at] = elems[i];
        }

        at++;
    }

    ((DatMap *) result)->size = at;
    return result;
}

/* Documented in header. */
zvalue datMapDel(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    if (index < 0) {
        return map;
    }

    zint size = mapSizeOf(map) - 1;
    zvalue result = allocMap(size);
    zmapping *elems = mapElems(result);
    zmapping *oldElems = mapElems(map);

    memcpy(elems, oldElems, index * sizeof(zmapping));
    memcpy(&elems[index], &oldElems[index + 1],
           (size - index) * sizeof(zmapping));
    return result;
}

/* Documented in header. */
zvalue datMapEmpty(void) {
    return allocMap(0);
}

/* Documented in header. */
zvalue datMapGet(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    return (index < 0) ? NULL : mapElems(map)[index].value;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue datMapNth(zvalue map, zint n) {
    datAssertMap(map);

    if (!datHasNth(map, n)) {
        return NULL;
    }

    if (mapSizeOf(map) == 1) {
        return map;
    }

    zmapping *mapping = &mapElems(map)[n];
    return mapFrom1(mapping->key, mapping->value);
}

/* Documented in header. */
zvalue datMapPut(zvalue map, zvalue key, zvalue value) {
    datAssertMap(map);
    datAssertValid(value);

    zint size = mapSizeOf(map);

    switch (size) {
        case 0: {
            datAssertValid(key);
            return mapFrom1(key, value);
        }
        case 1: {
            datAssertValid(key);
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
zvalue datMappingKey(zvalue map) {
    datAssertMapSize1(map);
    return mapElems(map)[0].key;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue datMappingValue(zvalue map) {
    datAssertMapSize1(map);
    return mapElems(map)[0].value;
}


/*
 * Type binding
 */

/* Documented in header. */
static void mapGcMark(zvalue map) {
    zint size = mapSizeOf(map);
    zmapping *elems = mapElems(map);

    for (zint i = 0; i < size; i++) {
        datMark(elems[i].key);
        datMark(elems[i].value);
    }
}

/* Documented in header. */
static bool mapEq(zvalue v1, zvalue v2) {
    zmapping *elems1 = mapElems(v1);
    zmapping *elems2 = mapElems(v2);
    zint sz1 = mapSizeOf(v1);
    zint sz2 = mapSizeOf(v2);

    if (sz1 != sz2) {
        return false;
    }

    for (zint i = 0; i < sz1; i++) {
        zmapping *e1 = &elems1[i];
        zmapping *e2 = &elems2[i];
        if (!(datEq(e1->key, e2->key) && datEq(e1->value, e2->value))) {
            return false;
        }
    }

    return true;
}

/* Documented in header. */
static zorder mapOrder(zvalue v1, zvalue v2) {
    zmapping *e1 = mapElems(v1);
    zmapping *e2 = mapElems(v2);
    zint sz1 = mapSizeOf(v1);
    zint sz2 = mapSizeOf(v2);
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

/* Documented in header. */
static DatType INFO_Map = {
    .id = DAT_MAP,
    .name = "Map",
    .sizeOf = mapSizeOf,
    .gcMark = mapGcMark,
    .gcFree = NULL,
    .eq = mapEq,
    .order = mapOrder
};
ztype DAT_Map = &INFO_Map;
