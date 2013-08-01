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
static zvalue genCollect = NULL;

/**
 * Does listification of an int. This returns a list of individual
 * bits (as ints).
 */
static zvalue listFromInt(zvalue intValue) {
    zvalue bit0 = constIntFromZint(0);
    zvalue bit1 = constIntFromZint(1);
    zint size = datSize(intValue);
    zint raw = datZintFromInt(intValue);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        zint bit = datZintGetBit(raw, i);
        arr[i] = (bit == 0) ? bit0 : bit1;
    }

    return datListFromArray(size, arr);
}

/**
 * Does (trivial) "listification" of a list. This returns the argument
 * unchanged.
 */
static zvalue listFromList(zvalue list) {
    return list;
}

/**
 * Does listification of a map. This returns a list of individual mappings.
 */
static zvalue listFromMap(zvalue map) {
    zint size = datSize(map);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = datMapNth(map, i);
    }

    return datListFromArray(size, arr);
}

/**
 * Does listification of a string. This returns a list of individual
 * characters.
 */
static zvalue listFromString(zvalue string) {
    zint size = datSize(string);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = constStringFromZchar(datStringNth(string, i));
    }

    return datListFromArray(size, arr);
}

/**
 * Does generator iteration to get a list.
 */
static zvalue collectGeneratorPerSe(zvalue generator) {
    zvalue arr[CONST_MAX_GENERATOR_ITEMS];
    zint at;

    zstackPointer save = datFrameStart();
    zvalue box = boxMutable();

    for (at = 0; /*at*/; at++) {
        zvalue nextGen = datFnCall(generator, 1, &box);

        if (nextGen == NULL) {
            break;
        } else if (at == CONST_MAX_GENERATOR_ITEMS) {
            die("Generator produced too many interpolated items.");
        }

        arr[at] = boxGet(box);
        generator = nextGen;
        boxReset(box);
    }

    zvalue result = datListFromArray(at, arr);
    datFrameReturn(save, result);
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
void generatorInit(void) {
    genCollect = datGenWith(STR_COLLECT);
    datGenBindCore(genCollect, DAT_Int,      listFromInt, NULL);
    datGenBindCore(genCollect, DAT_List,     listFromList, NULL);
    datGenBindCore(genCollect, DAT_Map,      listFromMap, NULL);
    datGenBindCore(genCollect, DAT_String,   listFromString, NULL);
    datGenBindCore(genCollect, DAT_Function, collectGeneratorPerSe, NULL);
    datGenSeal(genCollect);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue constCollectGenerator(zvalue value) {
    switch (datTypeId(value)) {
        case DAT_INT: {
            return listFromInt(value);
        }
        case DAT_LIST: {
            // Trivial pass-through.
            return value;
        }
        case DAT_MAP: {
            return listFromMap(value);
        }
        case DAT_STRING: {
            return listFromString(value);
        }
        case DAT_FUNCTION: {
            return collectGeneratorPerSe(value);
        }
        default: {
            die("Invalid type for constCollectGenerator.");
        }
    }
}
