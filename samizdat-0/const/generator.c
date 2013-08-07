/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator glue
 */

#include "impl.h"
#include "util.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** Generic function for `collect` (convert to list) dispatch. */
static zvalue GFN_collect = NULL;

/**
 * Does listification of an int. This returns a list of individual
 * bits (as ints).
 */
static zvalue collectInt(zvalue state, zint argc, const zvalue *args) {
    zvalue intValue = args[0];

    zint size = pbSize(intValue);
    zint raw = zintFromInt(intValue);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        zint bit = zintGetBit(raw, i);
        arr[i] = (bit == 0) ? PB_0 : PB_1;
    }

    return listFromArray(size, arr);
}

/**
 * Does (trivial) "listification" of a list. This returns the argument
 * unchanged.
 */
static zvalue collectList(zvalue state, zint argc, const zvalue *args) {
    return args[0];
}

/**
 * Does listification of a map. This returns a list of individual mappings.
 */
static zvalue collectMap(zvalue state, zint argc, const zvalue *args) {
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
static zvalue collectString(zvalue state, zint argc, const zvalue *args) {
    zvalue string = args[0];

    zint size = pbSize(string);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = constStringFromZchar(stringNth(string, i));
    }

    return listFromArray(size, arr);
}

/**
 * Does generator iteration to get a list.
 */
static zvalue collectGenerator(zvalue state, zint argc,
        const zvalue *args) {
    zvalue generator = args[0];

    zvalue arr[CONST_MAX_GENERATOR_ITEMS];
    zint at;

    zstackPointer save = pbFrameStart();
    zvalue box = boxMutable();

    for (at = 0; /*at*/; at++) {
        zvalue nextGen = fnCall(generator, 1, &box);

        if (nextGen == NULL) {
            break;
        } else if (at == CONST_MAX_GENERATOR_ITEMS) {
            die("Generator produced too many interpolated items.");
        }

        arr[at] = boxGet(box);
        generator = nextGen;
        boxReset(box);
    }

    zvalue result = listFromArray(at, arr);
    pbFrameReturn(save, result);
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
void generatorInit(void) {
    GFN_collect = gfnFrom(1, 1, STR_COLLECT);
    gfnBindCore(GFN_collect, TYPE_Int,      collectInt);
    gfnBindCore(GFN_collect, TYPE_List,     collectList);
    gfnBindCore(GFN_collect, TYPE_Map,      collectMap);
    gfnBindCore(GFN_collect, TYPE_String,   collectString);
    gfnBindCore(GFN_collect, TYPE_Function, collectGenerator);
    gfnBindCore(GFN_collect, TYPE_Generic,  collectGenerator);
    gfnSeal(GFN_collect);
    pbImmortalize(GFN_collect);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue constCollectGenerator(zvalue value) {
    return fnCall(GFN_collect, 1, &value);
}
