/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator glue
 */

#include "const.h"
#include "impl.h"
#include "util.h"

/*
 * Helper functions
 */

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
static zvalue listFromGeneratorPerSe(zvalue generator) {
    die("TODO");
    // zstackPointer save = datFrameStart();
    // langCall(function, 0, NULL);
    // datFrameReturn(save, NULL);
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue listFromGenerator(zvalue value) {
    switch (datType(value)) {
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
        case DAT_UNIQLET: {
            return listFromGeneratorPerSe(value);
        }
        default: {
            die("Invalid type for listFromGenerator.");
        }
    }
}
