/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator protocol
 */

#include "impl.h"
#include "type/Box.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Private definitions
 */

/**
 * Common implementation for `Value.collect` and `Value.filter`.
 */
static zvalue collectOrFilter(zvalue generator, zvalue function) {
    zvalue stackArr[DAT_SOFT_MAX_GENERATOR_ITEMS];
    zvalue *arr = stackArr;
    zint maxSize = DAT_SOFT_MAX_GENERATOR_ITEMS;
    zint at = 0;

    zstackPointer save = datFrameStart();
    zvalue box = makeCell(NULL);

    for (;;) {
        zvalue nextGen = GFN_CALL(nextValue, generator, box);

        if (nextGen == NULL) {
            break;
        }

        zvalue one = GFN_CALL(fetch, box);
        generator = nextGen;

        // Ideally, we wouldn't reuse the box (we'd use N yield boxes), but
        // for the sake of efficiency, we use the same box but reset it for
        // each iteration.
        GFN_CALL(store, box);

        if (function != NULL) {
            one = FUN_CALL(function, one);
            if (one == NULL) {
                continue;
            }
        } else if (one == NULL) {
            die("Unexpected lack of result.");
        }

        if (at == maxSize) {
            if (arr == stackArr) {
                maxSize = DAT_HARD_MAX_GENERATOR_ITEMS;
                arr = utilAlloc(maxSize * sizeof(zvalue));
                memcpy(arr, stackArr, at * sizeof(zvalue));
            } else {
                die("Generator produced way too many items.");
            }
        }

        arr[at] = one;
        at++;
    }

    zvalue result = listFromArray(at, arr);
    datFrameReturn(save, result);

    if (arr != stackArr) {
        utilFree(arr);
    }

    return result;
}


/*
 * Type Definition: `Generator`
 *
 * This includes bindings for the methods on all the core types.
 */

/* Documented in header. */
METH_IMPL(List, collect) {
    zvalue list = args[0];

    // Easy case: A list is its own collected result.
    return list;
}

/* Documented in header. */
METH_IMPL(Map, collect) {
    zvalue map = args[0];

    zint size = collSize(map);
    zmapping mappings[size];
    zvalue arr[size];

    arrayFromMap(mappings, map);

    for (zint i = 0; i < size; i++) {
        arr[i] = mapFromArray(1, &mappings[i]);
    }

    return listFromArray(size, arr);
}

/* Documented in header. */
METH_IMPL(String, collect) {
    zvalue string = args[0];

    zint size = collSize(string);
    zchar chars[size];
    zvalue arr[size];

    zcharsFromString(chars, string);

    for (zint i = 0; i < size; i++) {
        arr[i] = stringFromZchar(chars[i]);
    }

    return listFromArray(size, arr);
}

/**
 * Does generator iteration to get a list. This is what's bound to type
 * `Value`, on the assumption that the value in question has a binding
 * for the generic `nextValue`, which this function uses.
 */
METH_IMPL(Value, collect) {
    zvalue generator = args[0];

    return collectOrFilter(generator, NULL);
}

/* Documented in header. */
METH_IMPL(Map, filter) {
    zvalue map = args[0];
    zvalue function = args[1];
    zvalue unfiltered = GFN_CALL(collect, map);

    return GFN_CALL(filter, unfiltered, function);
}

/* Documented in header. */
METH_IMPL(Collection, filter) {
    zvalue coll = args[0];
    zvalue function = args[1];
    zint size = collSize(coll);
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = seqNth(coll, i);
        zvalue one = FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

/**
 * Does generator filtering to get a list. This is what's bound to type
 * `Value`, on the assumption that the value in question has a binding
 * for the generic `nextValue`, which this function uses.
 */
METH_IMPL(Value, filter) {
    zvalue generator = args[0];
    zvalue function = args[1];

    return collectOrFilter(generator, function);
}

/** Initializes the module. */
MOD_INIT(Generator) {
    MOD_USE(Box);
    MOD_USE(List);
    MOD_USE(Map);
    MOD_USE(String);

    GFN_collect = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "collect"));
    datImmortalize(GFN_collect);

    GFN_filter = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "filter"));
    datImmortalize(GFN_filter);

    GFN_nextValue = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nextValue"));
    datImmortalize(GFN_nextValue);

    METH_BIND(List,   collect);
    METH_BIND(Map,    collect);
    METH_BIND(String, collect);
    METH_BIND(Value,  collect);

    METH_BIND(Map,    filter);
    METH_BIND(Value,  filter);
    genericBindPrim(GFN_filter, TYPE_List,   Collection_filter, "List.filter");
    genericBindPrim(GFN_filter, TYPE_String, Collection_filter, "String.filter");
}

/* Documented in header. */
zvalue GFN_collect = NULL;

/* Documented in header. */
zvalue GFN_filter = NULL;

/* Documented in header. */
zvalue GFN_nextValue = NULL;
