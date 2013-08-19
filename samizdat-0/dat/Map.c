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
 * Gets a pointer to the value's info.
 */
static MapInfo *getInfo(zvalue list) {
    return pbPayload(list);
}

/**
 * Allocates a map of the given size.
 */
static zvalue allocMap(zint size) {
    zvalue result =
        pbAllocValue(TYPE_Map, sizeof(MapInfo) + size * sizeof(zmapping));

    getInfo(result)->size = size;
    return result;
}

/**
 * Constructs and returns a single-mapping map.
 */
static zvalue makeMapping(zvalue key, zvalue value) {
    zvalue result = allocMap(1);
    zmapping *elems = getInfo(result)->elems;

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
    zorder comp = pbOrder(k1, k2);

    if (comp == ZSAME) {
        return makeMapping(k2, v2);
    }

    zvalue result = allocMap(2);
    zmapping *elems = getInfo(result)->elems;

    if (comp == ZLESS) {
        elems[0].key = k1;
        elems[0].value = v1;
        elems[1].key = k2;
        elems[1].value = v2;
    } else {
        elems[0].key = k2;
        elems[0].value = v2;
        elems[1].key = k1;
        elems[1].value = v1;
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

    assertMap(map);
    pbAssertValid(key);

    entry->map = map;
    entry->key = key;

    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;
    zint min = 0;
    zint max = info->size - 1;

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
void assertMap(zvalue value) {
    assertHasType(value, TYPE_Map);
}

/* Documented in header. */
void assertMapSize1(zvalue value) {
    assertMap(value);
    if (getInfo(value)->size != 1) {
        die("Not a size 1 map.");
    }
}

/* Documented in header. */
void arrayFromMap(zmapping *result, zvalue map) {
    assertMap(map);

    MapInfo *info = getInfo(map);
    memcpy(result, info->elems, info->size * sizeof(zmapping));
}

/* Documented in header. */
zvalue mapCat(zvalue map1, zvalue map2) {
    assertMap(map1);
    assertMap(map2);

    MapInfo *info2 = getInfo(map2);
    zint size2 = info2->size;

    if (getInfo(map1)->size == 0) {
        return map2;
    } else if (size2 == 0) {
        return map1;
    }

    return mapCatArray(map1, size2, info2->elems);
}

/* Documented in header. */
zvalue mapCatArray(zvalue map, zint size, const zmapping *mappings) {
    assertMap(map);

    if (size == 0) {
        return map;
    } else if (size == 1) {
        return mapPut(map, mappings[0].key, mappings[0].value);
    }

    MapInfo *info = getInfo(map);
    zint resultSize = info->size + size;
    zvalue result = allocMap(resultSize);
    MapInfo *resultInfo = getInfo(result);
    zmapping *resultElems = resultInfo->elems;

    // Add all the mappings to the result, and sort it using mergesort.
    // Mergesort is stable and operates best on sorted data, and as it
    // happens the starting map is guaranteed to be sorted.

    memcpy(resultElems, info->elems, info->size * sizeof(zmapping));
    memcpy(&resultElems[info->size], mappings, size * sizeof(zmapping));
    mergesort(resultElems, resultSize, sizeof(zmapping), mappingOrder);

    // Remove all but the last of any sequence of equal-keys mappings.
    // The last one is preferred, since by construction that's the last
    // of any equal keys from the newly-added mappings.

    zint at = 1;
    for (zint i = 1; i < resultSize; i++) {
        if (pbEq(resultElems[i].key, resultElems[at-1].key)) {
            at--;
        }

        if (at != i) {
            resultElems[at] = resultElems[i];
        }

        at++;
    }

    resultInfo->size = at; // In case there were duplicate keys.
    return result;
}

/* Documented in header. */
zvalue mapDel(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    if (index < 0) {
        return map;
    }

    MapInfo *info = getInfo(map);
    zint size = info->size - 1;

    if (size == 0) {
        return EMPTY_MAP;
    }

    zvalue result = allocMap(size);
    zmapping *elems = getInfo(result)->elems;
    zmapping *oldElems = info->elems;

    memcpy(elems, oldElems, index * sizeof(zmapping));
    memcpy(&elems[index], &oldElems[index + 1],
           (size - index) * sizeof(zmapping));
    return result;
}

/* Documented in header. */
zvalue mapGet(zvalue map, zvalue key) {
    zint index = mapFind(map, key);

    return (index < 0) ? NULL : getInfo(map)->elems[index].value;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mapNth(zvalue map, zint n) {
    assertMap(map);

    MapInfo *info = getInfo(map);
    zint size = info->size;

    if ((n < 0) || (n >= size)) {
        return NULL;
    }

    if (size == 1) {
        return map;
    }

    zmapping *m = &info->elems[n];
    return makeMapping(m->key, m->value);
}

/* Documented in header. */
zvalue mapPut(zvalue map, zvalue key, zvalue value) {
    assertMap(map);
    pbAssertValid(value);

    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;
    zint size = info->size;

    switch (size) {
        case 0: {
            pbAssertValid(key);
            return makeMapping(key, value);
        }
        case 1: {
            pbAssertValid(key);
            zmapping *elems = info->elems;
            return mapFrom2(elems[0].key, elems[0].value, key, value);
        }
    }

    zint index = mapFind(map, key);
    zvalue result;

    if (index >= 0) {
        // The key exists in the given map, so we need to perform
        // a replacement.
        result = allocMap(size);
        memcpy(getInfo(result)->elems, elems, size * sizeof(zmapping));
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = allocMap(size + 1);

        zmapping *resultElems = getInfo(result)->elems;

        memcpy(resultElems, elems, index * sizeof(zmapping));
        memcpy(&resultElems[index + 1], &elems[index],
               (size - index) * sizeof(zmapping));
    }

    zmapping *elem = &getInfo(result)->elems[index];
    elem->key = key;
    elem->value = value;
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mappingKey(zvalue map) {
    assertMapSize1(map);
    return getInfo(map)->elems[0].key;
}

/* Documented in Samizdat Layer 0 spec. */
zvalue mappingValue(zvalue map) {
    assertMapSize1(map);
    return getInfo(map)->elems[0].value;
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
    MapInfo *info1 = getInfo(v1);
    MapInfo *info2 = getInfo(v2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 != size2) {
        return NULL;
    }

    zmapping *elems1 = info1->elems;
    zmapping *elems2 = info2->elems;

    for (zint i = 0; i < size1; i++) {
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
    MapInfo *info = getInfo(map);
    zint size = info->size;
    zmapping *elems = info->elems;

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
    MapInfo *info1 = getInfo(v1);
    MapInfo *info2 = getInfo(v2);
    zmapping *e1 = info1->elems;
    zmapping *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zorder result = pbOrder(e1[i].key, e2[i].key);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    if (size1 < size2) {
        return PB_NEG1;
    } else if (size1 > size2) {
        return PB_1;
    }

    for (zint i = 0; i < size; i++) {
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
    return intFromZint(getInfo(map)->size);
}

/* Documented in header. */
void datBindMap(void) {
    TYPE_Map = coreTypeFromName(stringFromUtf8(-1, "Map"), false);
    METH_BIND(Map, eq);
    METH_BIND(Map, gcMark);
    METH_BIND(Map, order);
    METH_BIND(Map, size);

    EMPTY_MAP = allocMap(0);
    pbImmortalize(EMPTY_MAP);
}

/* Documented in header. */
zvalue TYPE_Map = NULL;
