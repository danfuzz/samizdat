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
 * Type Definition: `Generator`
 *
 * This includes bindings for the methods on all the core types.
 */

/* Documented in header. */
METH_IMPL(Map, collect) {
    zvalue map = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;
    zint size = collSize(map);
    zmapping mappings[size];
    zvalue result[size];
    zint at = 0;

    arrayFromMap(mappings, map);

    for (zint i = 0; i < size; i++) {
        zvalue elem = mapFromArray(1, &mappings[i]);
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

/* Documented in header. */
METH_IMPL(Collection, collect) {
    zvalue coll = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;
    zint size = collSize(coll);
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = seqNth(coll, i);
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

/**
 * Does generator collection/filtering to get a list. This is what's bound
 * to type `Value`, on the assumption that the value in question has a binding
 * for the generic `nextValue`, which this function uses.
 */
METH_IMPL(Value, collect) {
    zvalue generator = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;
    zvalue stackArr[DAT_MAX_GENERATOR_ITEMS_SOFT];
    zvalue *arr = stackArr;
    zint maxSize = DAT_MAX_GENERATOR_ITEMS_SOFT;
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
                maxSize = DAT_MAX_GENERATOR_ITEMS_HARD;
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

/** Initializes the module. */
MOD_INIT(Generator) {
    MOD_USE(Box);
    MOD_USE(List);
    MOD_USE(Map);
    MOD_USE(String);

    GFN_collect = makeGeneric(1, 2, GFN_NONE, stringFromUtf8(-1, "collect"));
    datImmortalize(GFN_collect);

    GFN_nextValue = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nextValue"));
    datImmortalize(GFN_nextValue);

    METH_BIND(Map,    collect);
    METH_BIND(Value,  collect);
    genericBindPrim(GFN_collect, TYPE_List,   Collection_collect, "List.collect");
    genericBindPrim(GFN_collect, TYPE_String, Collection_collect, "String.collect");
}

/* Documented in header. */
zvalue GFN_collect = NULL;

/* Documented in header. */
zvalue GFN_nextValue = NULL;
