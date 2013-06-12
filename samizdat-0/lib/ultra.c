/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Helper for `makeMap` and `makeMapReverse`, which does most of the work.
 */
static zvalue makeMap1(zint argCount, const zvalue *args, bool reversed) {
    // Count the number of mappings, that will be used when building a map,
    // which can be either bigger or smaller than `argCount` (or the same).
    // Also validates that all arguments are maps or appropriate-sized lists.

    zint size = 0;
    for (zint i = 0; i < argCount; i++) {
        zvalue one = args[i];
        zint oneSize = datSize(one);

        switch (datType(one)) {
            case DAT_MAP: {
                size += oneSize;
                break;
            }
            case DAT_LIST: {
                if (oneSize < 2) {
                    die("Invalid mapping argument: list of size %lld", oneSize);
                }
                size += oneSize - 1;
                break;
            }
            default: {
                die("Invalid mapping argument.");
            }
        }
    }

    if (size == 0) {
        return EMPTY_MAP;
    }

    zmapping mappings[size];
    zint at = reversed ? size : 0;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = args[i];
        zint oneSize = datSize(one);

        switch (datType(one)) {
            case DAT_MAP: {
                if (reversed) { at -= oneSize; }
                datArrayFromMap(&mappings[at], one);
                if (!reversed) { at += oneSize; }
                break;
            }
            case DAT_LIST: {
                zvalue value = datListNth(one, 0);
                for (zint j = 1; j < oneSize; j++) {
                    if (reversed) { at--; }
                    mappings[at].key = datListNth(one, j);
                    mappings[at].value = value;
                    if (!reversed) { at++; }
                }
                break;
            }
            default: {
                die("Shouldn't happen.");
            }
        }
    }

    return datMapAddArray(EMPTY_MAP, size, mappings);
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeList) {
    if (argCount == 0) {
        return EMPTY_LIST;
    }

    return datListFromArray(argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMap) {
    return makeMap1(argCount, args, false);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMapReversed) {
    return makeMap1(argCount, args, true);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeRange) {
    requireExactly(argCount, 2);

    zvalue start = args[0];
    zvalue end = args[1];
    ztype type = datType(start);

    if (type != datType(end)) {
        die("Type mismatch on range.");
    }

    if (datOrder(start, end) > 0) {
        die("Bad order for range.");
    }

    if (type == DAT_INT) {
        zint startInt = datZintFromInt(start);
        zint endInt = datZintFromInt(end);
        zint size = endInt - startInt + 1;
        zvalue values[size];

        for (zint i = 0; i < size; i++) {
            values[i] = constIntFromZint(startInt + i);
        }

        return datListFromArray(size, values);
    } else if (type == DAT_STRING) {
        datAssertStringSize1(start);
        datAssertStringSize1(end);
        zchar startCh = datStringNth(start, 0);
        zchar endCh = datStringNth(end, 0);
        zint size = endCh - startCh + 1;
        zvalue values[size];

        for (zint i = 0; i < size; i++) {
            values[i] = constStringFromZchar(startCh + i);
        }

        return datListFromArray(size, values);
    }

    die("Bad type for range.");
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeToken) {
    requireRange(argCount, 1, 2);

    zvalue value = (argCount == 2) ? args[1] : NULL;
    return constTokenFrom(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeUniqlet) {
    requireExactly(argCount, 0);
    return datUniqlet();
}
