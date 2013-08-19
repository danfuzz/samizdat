/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator glue
 */

#include "impl.h"
#include "type/Box.h"
#include "type/Callable.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "util.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/** Generic function for `collect` (convert to list) dispatch. */
static zvalue GFN_collect = NULL;

/**
 * Does listification of an int. This returns a list of individual
 * bits (as ints).
 */
METH_IMPL(Int, collect) {
    zvalue intValue = args[0];

    zint size = pbSize(intValue);
    zint raw = zintFromInt(intValue);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        zint bit;
        zintBit(&bit, raw, i); // Always succeeds.
        arr[i] = (bit == 0) ? PB_0 : PB_1;
    }

    return listFromArray(size, arr);
}

/**
 * Does (trivial) "listification" of a list. This returns the argument
 * unchanged.
 */
METH_IMPL(List, collect) {
    return args[0];
}

/**
 * Does listification of a map. This returns a list of individual mappings.
 */
METH_IMPL(Map, collect) {
    zvalue map = args[0];

    zint size = pbSize(map);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = mapNth(map, i);
    }

    return listFromArray(size, arr);
}

/**
 * Does listification of a string. This returns a list of individual
 * characters.
 */
METH_IMPL(String, collect) {
    zvalue string = args[0];

    zint size = pbSize(string);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = stringFromZchar(stringNth(string, i));
    }

    return listFromArray(size, arr);
}

/**
 * Does generator iteration to get a list. This is what's bound to type
 * `Value`, on the assumption that the value in question has a binding
 * for the generic `call`, which this function uses.
 */
METH_IMPL(Value, collect) {
    zvalue generator = args[0];

    zvalue arr[CONST_MAX_GENERATOR_ITEMS];
    zint at;

    zstackPointer save = pbFrameStart();
    zvalue box = makeMutableBox(NULL);

    for (at = 0; /*at*/; at++) {
        zvalue nextGen = FUN_CALL(generator, box);

        if (nextGen == NULL) {
            break;
        } else if (at == CONST_MAX_GENERATOR_ITEMS) {
            die("Generator produced too many interpolated items.");
        }

        arr[at] = GFN_CALL(fetch, box);
        generator = nextGen;

        // Ideally, we wouldn't reuse the box (we'd just use N yield boxes),
        // but for the sake of efficiency, we use the same box but reset it
        // for each iteration.
        GFN_CALL(store, box);
    }

    zvalue result = listFromArray(at, arr);
    pbFrameReturn(save, result);
    return result;
}


/*
 * Module Definitions
 */

/* Documented in header. */
void generatorInit(void) {
    GFN_collect = makeGeneric(1, 1, stringFromUtf8(-1, "collect"));
    pbImmortalize(GFN_collect);

    METH_BIND(Int, collect);
    METH_BIND(List, collect);
    METH_BIND(Map, collect);
    METH_BIND(String, collect);
    METH_BIND(Value, collect);
    genericSeal(GFN_collect);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue constCollectGenerator(zvalue value) {
    return GFN_CALL(collect, value);
}
