// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Box.h"
#include "type/Generator.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Record.h"
#include "type/SymbolTable.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

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
static MapInfo *getInfo(zvalue map) {
    return datPayload(map);
}

/**
 * Allocates a map of the given size.
 */
static zvalue allocMap(zint size) {
    zvalue result =
        datAllocValue(CLS_Map, sizeof(MapInfo) + size * sizeof(zmapping));

    getInfo(result)->size = size;
    return result;
}

/**
 * Constructs and returns a map with the given mappings, without doing
 * any processing (checking or sorting) on the mappings.
 */
static zvalue mapFromArrayUnchecked(zint size, zmapping *mappings) {
    zvalue result = allocMap(size);
    MapInfo *info = getInfo(result);

    utilCpy(zmapping, info->elems, mappings, size);
    return result;
}

/**
 * Constructs and returns a single-mapping map.
 */
static zvalue mapFrom1(zmapping elem) {
    return mapFromArrayUnchecked(1, &elem);
}

/**
 * Allocates and returns a map with up to two mappings. This will return a
 * single-mapping map if the two keys are the same, in which case the *second*
 * value is used.
 */
static zvalue mapFrom2(zmapping elem1, zmapping elem2) {
    switch (cm_order(elem1.key, elem2.key)) {
        case ZLESS: {
            return mapFromArrayUnchecked(2, (zmapping[]) {elem1, elem2});
        }
        case ZMORE: {
            return mapFromArrayUnchecked(2, (zmapping[]) {elem2, elem1});
        }
        default: {
            return mapFrom1(elem2);
        }
    }
}

/**
 * Given a map, find the index of the given key. `map` must be a map.
 * Returns the index of the key if found. If not found, then this returns
 * `~insertionIndex` (a negative number).
 */
static zint mapFind(zvalue map, zvalue key) {
    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;

    // Take care of a couple trivial cases.
    switch (info->size) {
        case 0: {
            return ~0;
        }
        case 1: {
            switch (cm_order(key, elems[0].key)) {
                case ZLESS: { return ~0; }
                case ZMORE: { return ~1; }
                default:    { return 0;  }
            }
        }
    }

    MapCacheEntry *entry = mapGetCacheEntry(map, key);

    if ((entry->map == map) && (entry->key == key)) {
        return entry->index;
    }

    entry->map = map;
    entry->key = key;

    zint min = 0;
    zint max = info->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (cm_order(key, elems[guess].key)) {
            case ZLESS: { max = guess - 1; break; }
            case ZMORE: { min = guess + 1; break; }
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
    return cm_order(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromMap(zmapping *result, zvalue map) {
    assertHasClass(map, CLS_Map);

    MapInfo *info = getInfo(map);
    utilCpy(zmapping, result, info->elems, info->size);
}

// Documented in header.
zvalue mapFromArray(zint size, zmapping *mappings) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < size; i++) {
            assertValid(mappings[i].key);
            assertValid(mappings[i].value);
        }
    }

    // Handle special cases that are particularly easy.
    switch (size) {
        case 0: { return EMPTY_MAP;                          }
        case 1: { return mapFrom1(mappings[0]);              }
        case 2: { return mapFrom2(mappings[0], mappings[1]); }
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
    return mapFromArrayUnchecked(at, mappings);
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_1(Map, castFrom, value) {
    zvalue cls = classOf(value);

    if (valEq(cls, CLS_SymbolTable)) {
        zint size = symbolTableSize(value);
        zmapping mappings[size];
        arrayFromSymbolTable(mappings, value);
        return mapFromArray(size, mappings);
    } else if (classAccepts(thsClass, value)) {
        return value;
    }

    return NULL;
}

// Documented in spec.
CMETH_IMPL_rest(Map, new, args) {
    if ((argsSize & 1) != 0) {
        die("Odd argument count for map construction.");
    }

    zint size = argsSize >> 1;
    zmapping mappings[size];
    for (zint i = 0, at = 0; i < size; i++, at += 2) {
        mappings[i] = (zmapping) {args[at], args[at + 1]};
    }

    return mapFromArray(size, mappings);
}

// Documented in spec.
CMETH_IMPL_1_rest(Map, singleValue, first, args) {
    if (argsSize == 0) {
        return EMPTY_MAP;
    }

    // The value is passed at the end of the argument list. We pull out the
    // value here, and manually set up the first mapping.

    zmapping mappings[argsSize];
    zvalue value = args[argsSize - 1];

    mappings[0] = (zmapping) {first, value};

    for (zint i = 1; i < argsSize; i++) {
        mappings[i] = (zmapping) {args[i - 1], value};
    }

    return mapFromArray(argsSize, mappings);
}

// Documented in spec.
METH_IMPL_1(Map, castToward, cls) {
    MapInfo *info = getInfo(ths);

    if (valEq(cls, CLS_SymbolTable)) {
        zint size = info->size;
        return symbolTableFromArray(info->size, info->elems);
    } else if (classAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_rest(Map, cat, args) {
    if (argsSize == 0) {
        return ths;
    }

    zint thsSize = getInfo(ths)->size;
    zvalue maps[argsSize];

    zint size = thsSize;
    for (zint i = 0; i < argsSize; i++) {
        zvalue one = args[i];
        if (classAccepts(CLS_SymbolTable, one)) {
            one = METH_CALL(castFrom, CLS_Map, one);
        } else {
            assertHasClass(one, CLS_Map);
        }
        maps[i] = one;
        size += getInfo(one)->size;
    }

    zmapping elems[size];
    zint at = thsSize;
    arrayFromMap(elems, ths);
    for (zint i = 0; i < argsSize; i++) {
        arrayFromMap(&elems[at], maps[i]);
        at += getInfo(maps[i])->size;
    }

    return mapFromArray(size, elems);
}

// Documented in spec.
METH_IMPL_0_1(Map, collect, function) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zmapping mappings[size];
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = mapFromArray(1, &info->elems[i]);
        zvalue one = (function == NULL)
            ? elem
            : FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

// Documented in spec.
METH_IMPL_1(Map, del, key) {
    zint index = mapFind(ths, key);

    if (index < 0) {
        return ths;
    }

    MapInfo *info = getInfo(ths);
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

// Documented in spec.
METH_IMPL_0(Map, fetch) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;

    switch (size) {
        case 0: {
            return NULL;
        }
        case 1: {
            return ths;
        }
        default: {
            die("Invalid to call `fetch` on map with size > 1.");
        }
    }
}

// Documented in header.
METH_IMPL_0(Map, gcMark) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zmapping *elems = info->elems;

    for (zint i = 0; i < size; i++) {
        datMark(elems[i].key);
        datMark(elems[i].value);
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_1(Map, get, key) {
    zint index = mapFind(ths, key);
    return (index < 0) ? NULL : getInfo(ths)->elems[index].value;
}

// Documented in spec.
METH_IMPL_0(Map, get_key) {
    MapInfo *info = getInfo(ths);

    if (info->size != 1) {
        die("Not a size 1 map.");
    }

    return info->elems[0].key;
}

// Documented in spec.
METH_IMPL_0(Map, get_size) {
    return intFromZint(getInfo(ths)->size);
}

// Documented in spec.
METH_IMPL_0(Map, get_value) {
    MapInfo *info = getInfo(ths);

    if (info->size != 1) {
        die("Not a size 1 map.");
    }

    return info->elems[0].value;
}

// Documented in spec.
METH_IMPL_0(Map, keyList) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zmapping *elems = info->elems;
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = elems[i].key;
    }

    return listFromArray(size, arr);
}

// Documented in spec.
METH_IMPL_1(Map, nextValue, box) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;

    switch (size) {
        case 0: {
            // `map` is empty.
            return NULL;
        }
        case 1: {
            // `map` is a single element, so we can yield it directly.
            cm_store(box, ths);
            return EMPTY_MAP;
        }
        default: {
            // Make a mapping for the first element, yield it, and return
            // a map of the remainder.
            zmapping *elems = info->elems;
            zvalue mapping = mapFrom1(elems[0]);
            cm_store(box, mapping);
            return mapFromArrayUnchecked(size - 1, &elems[1]);
        }
    }
}

// Documented in spec.
METH_IMPL_1(Map, nthMapping, n) {
    MapInfo *info = getInfo(ths);
    zint index = seqNthIndexStrict(info->size, n);

    if (index < 0) {
        return NULL;
    } else if (info->size == 1) {
        return ths;
    } else {
        return mapFrom1(info->elems[index]);
    }
}

// Documented in spec.
METH_IMPL_2(Map, put, key, value) {
    MapInfo *info = getInfo(ths);
    zmapping *elems = info->elems;
    zint size = info->size;

    if (DAT_CONSTRUCTION_PARANOIA) {
        assertValid(key);
        assertValid(value);
    }

    switch (size) {
        case 0: {
            // `put({}, ...)`
            return mapFrom1((zmapping) {key, value});
        }
        case 1: {
            return mapFrom2(elems[0], (zmapping) {key, value});
        }
    }

    zint index = mapFind(ths, key);
    zvalue result;
    zmapping *resultElems;

    if (index >= 0) {
        // The key exists in the given map, so we need to perform
        // a replacement.
        result = allocMap(size);
        resultElems = getInfo(result)->elems;
        utilCpy(zmapping, getInfo(result)->elems, elems, size);
    } else {
        // The key wasn't found, so we need to insert a new one.
        index = ~index;
        result = allocMap(size + 1);
        resultElems = getInfo(result)->elems;
        utilCpy(zmapping, resultElems, elems, index);
        utilCpy(zmapping, &resultElems[index + 1], &elems[index],
            (size - index));
    }

    resultElems[index] = (zmapping) {key, value};
    return result;
}

// Documented in spec.
METH_IMPL_1(Map, totalEq, other) {
    assertHasClass(other, CLS_Map);  // Note: Not guaranteed to be a `Map`.
    MapInfo *info1 = getInfo(ths);
    MapInfo *info2 = getInfo(other);
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

    return ths;
}

// Documented in spec.
METH_IMPL_1(Map, totalOrder, other) {
    assertHasClass(other, CLS_Map);  // Note: Not guaranteed to be a `Map`.
    MapInfo *info1 = getInfo(ths);
    MapInfo *info2 = getInfo(other);
    zmapping *e1 = info1->elems;
    zmapping *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zorder result = cm_order(e1[i].key, e2[i].key);
        if (result != ZSAME) {
            return symbolFromZorder(result);
        }
    }

    if (size1 < size2) {
        return SYM(less);
    } else if (size1 > size2) {
        return SYM(more);
    }

    for (zint i = 0; i < size; i++) {
        zorder result = cm_order(e1[i].value, e2[i].value);
        if (result != ZSAME) {
            return symbolFromZorder(result);
        }
    }

    return SYM(same);
}

// Documented in spec.
METH_IMPL_0(Map, valueList) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zmapping *elems = info->elems;
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = elems[i].value;
    }

    return listFromArray(size, arr);
}

/** Initializes the module. */
MOD_INIT(Map) {
    MOD_USE(Generator);
    MOD_USE(List);
    MOD_USE(MapCache);

    CLS_Map = makeCoreClass(SYM(Map), CLS_Core,
        METH_TABLE(
            CMETH_BIND(Map, castFrom),
            CMETH_BIND(Map, new),
            CMETH_BIND(Map, singleValue)),
        METH_TABLE(
            METH_BIND(Map, castToward),
            METH_BIND(Map, cat),
            METH_BIND(Map, collect),
            METH_BIND(Map, del),
            METH_BIND(Map, fetch),
            METH_BIND(Map, gcMark),
            METH_BIND(Map, get),
            METH_BIND(Map, get_key),
            METH_BIND(Map, get_size),
            METH_BIND(Map, get_value),
            METH_BIND(Map, keyList),
            METH_BIND(Map, nextValue),
            METH_BIND(Map, nthMapping),
            METH_BIND(Map, put),
            METH_BIND(Map, totalEq),
            METH_BIND(Map, totalOrder),
            METH_BIND(Map, valueList)));

    EMPTY_MAP = datImmortalize(allocMap(0));
}

// Documented in header.
zvalue CLS_Map = NULL;

// Documented in header.
zvalue EMPTY_MAP = NULL;
