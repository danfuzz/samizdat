// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Box.h"
#include "type/Cmp.h"
#include "type/Generator.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Record.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "util.h"

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
            return mapFromMapping(elem2);
        }
    }
}

/**
 * Given a map and its info struct, find the index of the given key. Returns
 * the index of the key if found. If not found, then this returns
 * `~insertionIndex` (a negative number).
 */
static zint mapFind(zvalue map, MapInfo *info, zvalue key) {
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

    zint min = 0;
    zint max = info->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (cm_order(key, elems[guess].key)) {
            case ZLESS: { max = guess - 1; break; }
            case ZMORE: { min = guess + 1; break; }
            default: {
                return guess;
            }
        }
    }

    // Not found. The insert point is at `min`. Per the API, this is
    // represented as `~index` (and not, in particular, as `-index`)
    // so that an insertion point of `0` can be unambiguously
    // represented.

    return ~min;
}

/**
 * Mapping comparison function, passed to standard library sorting
 * functions.
 */
static int mappingOrder(const void *m1, const void *m2) {
    return cm_order(((zmapping *) m1)->key, ((zmapping *) m2)->key);
}

/**
 * Put a new mapping into a map, either adding or replacing a key. Returns
 * a new map. `map` is assumed to be a valid map.
 */
static zvalue putMapping(zvalue map, zmapping mapping) {
    MapInfo *info = getInfo(map);
    zmapping *elems = info->elems;
    zint size = info->size;

    switch (size) {
        case 0: {
            // `map` is empty (`{}`).
            return mapFromMapping(mapping);
        }
        case 1: {
            return mapFrom2(elems[0], mapping);
        }
    }

    zint index = mapFind(map, info, mapping.key);
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

    resultElems[index] = mapping;
    return result;
}


//
// Exported Definitions
//

// Documented in header.
zvalue mapFromArray(zint size, zmapping *mappings) {
    if (CLS_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < size; i++) {
            assertValid(mappings[i].key);
            assertValid(mappings[i].value);
        }
    }

    // Handle special cases that are particularly easy.
    switch (size) {
        case 0: { return EMPTY_MAP;                          }
        case 1: { return mapFromMapping(mappings[0]);        }
        case 2: { return mapFrom2(mappings[0], mappings[1]); }
    }

    // Sort the mappings using a stable sort. The stability matters due to
    // this function's API. `utilSortStable` is also written to work well on
    // partially-sorted data, and as it happens, the input to this function is
    // commonly partially sorted.

    utilSortStable(mappings, size, sizeof(zmapping), mappingOrder);

    // Collapse away all but the last of any sequence of same-key mappings.
    // The last one is kept, as that is consistent with the exposed API.

    zint at = 1;
    for (zint i = 1; i < size; i++) {
        if (cmpEq(mappings[i].key, mappings[at - 1].key)) {
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

// Documented in header.
zvalue mapFromMapping(zmapping mapping) {
    return mapFromArrayUnchecked(1, &mapping);
}

// Documented in header.
zassoc zassocFromMap(zvalue map) {
    assertHasClass(map, CLS_Map);

    MapInfo *info = getInfo(map);
    return (zassoc) {info->size, info->elems};
}



//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_1(Map, castFrom, value) {
    zvalue cls = classOf(value);

    if (cmpEq(cls, CLS_SymbolTable)) {
        // Do nothing here; fall through the `if` to the conversion below.
    } else if (cmpEq(cls, CLS_Record)) {
        value = get_data(value);
        // ...and fall through to the conversion below.
    } else if (typeAccepts(thsClass, value)) {
        return value;
    } else {
        return NULL;
    }

    // We were given either a `Record` or a `SymbolTable`.
    zint size = symtabSize(value);
    zmapping mappings[size];
    arrayFromSymtab(mappings, value);
    return mapFromArray(size, mappings);
}

// Documented in spec.
CMETH_IMPL_rest(Map, new, args) {
    if ((args.size & 1) != 0) {
        die("Odd argument count for map construction.");
    }

    zint size = args.size >> 1;
    zmapping mappings[size];
    for (zint i = 0, at = 0; i < size; i++, at += 2) {
        mappings[i] = (zmapping) {args.elems[at], args.elems[at + 1]};
    }

    return mapFromArray(size, mappings);
}

// Documented in spec.
CMETH_IMPL_rest_1(Map, singleValue, keys, value) {
    if (keys.size == 0) {
        return EMPTY_MAP;
    }

    zmapping mappings[keys.size];
    for (zint i = 0; i < keys.size; i++) {
        mappings[i] = (zmapping) {keys.elems[i], value};
    }

    return mapFromArray(keys.size, mappings);
}

// Documented in spec.
METH_IMPL_1(Map, castToward, cls) {
    MapInfo *info = getInfo(ths);

    if (cmpEq(cls, CLS_SymbolTable)) {
        zint size = info->size;
        return symtabFromZassoc((zassoc) {info->size, info->elems});
    } else if (typeAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_rest(Map, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    MapInfo *thsInfo = getInfo(ths);
    zint thsSize = thsInfo->size;

    zvalue maps[args.size];
    MapInfo *infos[args.size];
    zint size = thsSize;

    for (zint i = 0; i < args.size; i++) {
        // Note: `typeCast` guarantees that a non-null result is of the
        // indicated class.
        zvalue one = typeCast(CLS_Map, args.elems[i]);

        if (one == NULL) {
            die("Invalid argument to `cat()`: %s", cm_debugString(one));
        }

        maps[i] = one;
        infos[i] = getInfo(one);
        size += infos[i]->size;
    }

    // Special cases for efficiency.

    if (size == thsSize) {
        // All the arguments were empty.
        return ths;
    }

    if (args.size == 1) {
        if (thsSize == 0) {
            // This is `{}.cat(arg)` with a single argument.
            return maps[0];
        }

        MapInfo *info = infos[0];
        if (info->size == 1) {
            // This is `map.cat(arg)`, where `arg` is a single mapping.
            return putMapping(ths, info->elems[0]);
        }
    }

    // The general case.

    zmapping elems[size];
    zint at = thsSize;
    utilCpy(zmapping, elems, thsInfo->elems, thsSize);
    for (zint i = 0; i < args.size; i++) {
        zint oneSize = infos[i]->size;
        utilCpy(zmapping, &elems[at], infos[i]->elems, oneSize);
        at += oneSize;
    }

    return mapFromArray(size, elems);
}

// Documented in spec.
METH_IMPL_0_opt(Map, collect, function) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = mapFromMapping(info->elems[i]);
        zvalue one = (function == NULL)
            ? elem
            : FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromZarray((zarray) {at, result});
}

// Documented in spec.
METH_IMPL_1(Map, crossEq, other) {
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
        if (!(cmpEq(e1->key, e2->key) && cmpEq(e1->value, e2->value))) {
            return NULL;
        }
    }

    return ths;
}

// Documented in spec.
METH_IMPL_1(Map, crossOrder, other) {
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
METH_IMPL_rest(Map, del, keys) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zmapping elems[size];
    bool any = false;

    if ((keys.size == 0) || (size == 0)) {
        // Easy outs: Not actually deleting anything, and/or starting out
        // with the empty map.
        return ths;
    }

    // Make a local copy of the original mappings.
    utilCpy(zmapping, elems, info->elems, size);

    // Null out the `key` for any of the given `keys`.
    for (zint i = 0; i < keys.size; i++) {
        zint index = mapFind(ths, info, keys.elems[i]);
        if (index >= 0) {
            any = true;
            elems[index].key = NULL;
        }
    }

    if (! any) {
        // None of `keys` were in `ths`.
        return ths;
    }

    // Compact away the holes.
    zint at = 0;
    for (zint i = 0; i < size; i++) {
        if (elems[i].key != NULL) {
            if (i != at) {
                elems[at] = elems[i];
            }
            at++;
        }
    }

    if (at == 0) {
        // All of the elements were removed.
        return EMPTY_MAP;
    }

    // Construct a new map with the remaining elements. `elems` is already
    // sorted, so it's safe to skip the sorting step.
    return mapFromArrayUnchecked(at, elems);
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

// Documented in spec.
METH_IMPL_0_opt(Map, forEach, function) {
    MapInfo *info = getInfo(ths);
    zint size = info->size;
    zvalue result = NULL;

    if (function == NULL) {
        // Without a function, this method just returns the last element.
        return (size == 0) ? NULL : mapFromMapping(info->elems[size - 1]);
    }

    for (zint i = 0; i < size; i++) {
        zvalue v = FUN_CALL(function, mapFromMapping(info->elems[i]));
        if (v != NULL) {
            result = v;
        }
    }

    return result;
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
    MapInfo *info = getInfo(ths);
    zint index = mapFind(ths, info, key);
    return (index < 0) ? NULL : info->elems[index].value;
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

    return listFromZarray((zarray) {size, arr});
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
            zvalue mapping = mapFromMapping(elems[0]);
            cm_store(box, mapping);
            return mapFromArrayUnchecked(size - 1, &elems[1]);
        }
    }
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

    return listFromZarray((zarray) {size, arr});
}

/** Initializes the module. */
MOD_INIT(Map) {
    MOD_USE(Generator);

    CLS_Map = makeCoreClass(SYM(Map), CLS_Core,
        METH_TABLE(
            CMETH_BIND(Map, castFrom),
            CMETH_BIND(Map, new),
            CMETH_BIND(Map, singleValue)),
        METH_TABLE(
            METH_BIND(Map, castToward),
            METH_BIND(Map, cat),
            METH_BIND(Map, collect),
            METH_BIND(Map, crossEq),
            METH_BIND(Map, crossOrder),
            METH_BIND(Map, del),
            METH_BIND(Map, fetch),
            METH_BIND(Map, forEach),
            METH_BIND(Map, gcMark),
            METH_BIND(Map, get),
            METH_BIND(Map, get_key),
            METH_BIND(Map, get_size),
            METH_BIND(Map, get_value),
            METH_BIND(Map, keyList),
            METH_BIND(Map, nextValue),
            METH_BIND(Map, valueList)));

    EMPTY_MAP = datImmortalize(allocMap(0));
}

// Documented in header.
zvalue CLS_Map = NULL;

// Documented in header.
zvalue EMPTY_MAP = NULL;
