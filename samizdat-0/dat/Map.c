/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"

#include <stdlib.h>


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
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map. If not, this aborts the process
 * with a diagnostic message.
 */
static void assertMap(zvalue value) {
    assertHasType(value, TYPE_Map);
}

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
static void assertMapSize1(zvalue value) {
    assertMap(value);
    if (getInfo(value)->size != 1) {
        die("Not a size 1 map.");
    }
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
    zorder comp = valOrder(k1, k2);

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
    assertValid(key);

    entry->map = map;
    entry->key = key;

    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;
    zint min = 0;
    zint max = info->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (valOrder(key, elems[guess].key)) {
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
    return valOrder(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void arrayFromMap(zmapping *result, zvalue map) {
    assertMap(map);

    MapInfo *info = getInfo(map);
    utilCpy(zmapping, result, info->elems, info->size);
}

/* Documented in header. */
zvalue mapFromArray(zint size, zmapping *mappings) {
    // Handle special cases that are particularly easy.
    switch (size) {
        case 0: {
            return EMPTY_MAP;
        }
        case 1: {
            return makeMapping(mappings[0].key, mappings[0].value);
        }
        case 2: {
            return mapFrom2(
                mappings[0].key, mappings[0].value,
                mappings[1].key, mappings[1].value);
        }
    }

    // Sort the mappings using mergesort. Mergesort is stable and operates
    // best on partially sorted data. As it happens, the input to this
    // function is commonly partially sorted, and the stability matters
    // due to this function's API.

    mergesort(mappings, size, sizeof(zmapping), mappingOrder);

    // Collapse away all but the last of any sequence of same-key mappings.
    // The last one is kept, as that is consistent with the exposed API.

    zint at = 1;
    for (zint i = 1; i < size; i++) {
        if (valEq(mappings[i].key, mappings[at - 1].key)) {
            at--;
        }

        if (at != i) {
            mappings[at] = mappings[i];
        }

        at++;
    }

    // Allocate, populate, and return the result.

    zvalue result = allocMap(at);
    MapInfo *info = getInfo(result);

    utilCpy(zmapping, info->elems, mappings, at);
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
METH_IMPL(Map, cat) {
    if (argCount == 1) {
        return args[0];
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        size += getInfo(args[i])->size;
    }

    zmapping elems[size];

    for (zint i = 0, at = 0; i < argCount; i++) {
        arrayFromMap(&elems[at], args[i]);
        at += getInfo(args[i])->size;
    }

    return mapFromArray(size, elems);
}

/* Documented in header. */
METH_IMPL(Map, del) {
    zvalue map = args[0];
    zvalue key = args[1];

    zint index = mapFind(map, key);

    if (index < 0) {
        return map;
    }

    MapInfo *info = getInfo(map);
    zint size = info->size;

    if (size == 1) {
        return EMPTY_MAP;
    }

    zvalue result = allocMap(size - 1);
    zmapping *elems = getInfo(result)->elems;
    zmapping *oldElems = info->elems;

    utilCpy(zmapping, elems, oldElems, index);
    utilCpy(zmapping, &elems[index], &oldElems[index + 1], (size - index - 1));
    return result;
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
METH_IMPL(Map, get) {
    zvalue map = args[0];
    zvalue key = args[1];

    zint index = mapFind(map, key);
    return (index < 0) ? NULL : getInfo(map)->elems[index].value;
}

/* Documented in header. */
METH_IMPL(Map, keyList) {
    zvalue map = args[0];

    MapInfo *info = getInfo(map);
    zint size = info->size;
    zmapping *elems = info->elems;
    zmapping mappings[size];
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = elems[i].key;
    }

    return listFromArray(size, arr);
}

/* Documented in header. */
METH_IMPL(Map, nth) {
    zvalue map = args[0];
    zvalue n = args[1];

    MapInfo *info = getInfo(map);
    zint index = collNthIndexStrict(info->size, n);

    if (index < 0) {
        return NULL;
    }

    if (info->size == 1) {
        return map;
    }

    zmapping *m = &info->elems[index];
    return makeMapping(m->key, m->value);
}

/* Documented in header. */
METH_IMPL(Map, perEq) {
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
        if (!(valEq(e1->key, e2->key) && valEq(e1->value, e2->value))) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
METH_IMPL(Map, perOrder) {
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
        zorder result = valOrder(e1[i].key, e2[i].key);
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
        zorder result = valOrder(e1[i].value, e2[i].value);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    return PB_0;
}

/* Documented in header. */
METH_IMPL(Map, put) {
    zvalue map = args[0];
    zvalue key = args[1];
    zvalue value = args[2];

    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;
    zint size = info->size;

    switch (size) {
        case 0: {
            // `put([:], ...)`
            return makeMapping(key, value);
        }
        case 1: {
            return mapFrom2(elems[0].key, elems[0].value, key, value);
        }
    }

    zint index = mapFind(map, key);
    zvalue result;

    if (index >= 0) {
        // The key exists in the given map, so we need to perform
        // a replacement.
        result = allocMap(size);
        utilCpy(zmapping, getInfo(result)->elems, elems, size);
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = allocMap(size + 1);

        zmapping *resultElems = getInfo(result)->elems;

        utilCpy(zmapping, resultElems, elems, index);
        utilCpy(zmapping, &resultElems[index+1], &elems[index], (size - index));
    }

    zmapping *elem = &getInfo(result)->elems[index];
    elem->key = key;
    elem->value = value;
    return result;
}

/* Documented in header. */
METH_IMPL(Map, sizeOf) {
    zvalue map = args[0];
    return intFromZint(getInfo(map)->size);
}

/* Documented in header. */
METH_IMPL(Map, slice) {
    zvalue map = args[0];
    MapInfo *info = getInfo(map);
    zint start;
    zint end;

    collConvertSliceArgs(&start, &end, info->size, argCount, args);
    return mapFromArray(end - start, &info->elems[start]);
}

/* Documented in header. */
void datBindMap(void) {
    TYPE_Map = coreTypeFromName(stringFromUtf8(-1, "Map"), false);
    METH_BIND(Map, cat);
    METH_BIND(Map, del);
    METH_BIND(Map, gcMark);
    METH_BIND(Map, get);
    METH_BIND(Map, keyList);
    METH_BIND(Map, nth);
    METH_BIND(Map, perEq);
    METH_BIND(Map, perOrder);
    METH_BIND(Map, put);
    METH_BIND(Map, sizeOf);
    METH_BIND(Map, slice);

    EMPTY_MAP = allocMap(0);
    pbImmortalize(EMPTY_MAP);
}

/* Documented in header. */
zvalue TYPE_Map = NULL;
