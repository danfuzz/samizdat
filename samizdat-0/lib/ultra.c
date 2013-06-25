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

/**
 * Helper for `makeRange*`, which does most of the work.
 */
static zvalue makeRange1(zint argCount, const zvalue *args, bool inclusive) {
    requireRange(argCount, 2, 3);

    zvalue first = args[0];
    zvalue limit = args[1];
    ztype type = datType(first);
    bool ints;
    zint firstInt;
    zint limitInt;
    zint increment;

    if (type != datType(limit)) {
        die("Type mismatch on range.");
    } else if (type == DAT_INT) {
        ints = true;
    } else if (type == DAT_STRING) {
        ints = false;
    } else {
        die("Bad type for range.");
    }

    if (ints) {
        firstInt = datZintFromInt(first);
        limitInt = datZintFromInt(limit);
    } else {
        datAssertStringSize1(first);
        datAssertStringSize1(limit);
        firstInt = datStringNth(first, 0);
        limitInt = datStringNth(limit, 0);
    }

    if (argCount == 3) {
        increment = datZintFromInt(args[2]);
        if (increment == 0) {
            // Arrange for this special case to work out below.
            limitInt = firstInt;
            increment = 1;
            inclusive = true;
        }
    } else {
        increment = 1;
    }

    if (!inclusive) {
        limitInt += (increment > 0) ? -1 : 1;
    }

    zint size = (limitInt + increment - firstInt) / increment;

    if (size <= 0) {
        return EMPTY_LIST;
    }

    zvalue values[size];

    for (zint i = 0, v = firstInt; i < size; i++, v += increment) {
        values[i] =
            ints ? constIntFromZint(v) : constStringFromZchar((zchar) v);
    }

    return datListFromArray(size, values);
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
PRIM_IMPL(makeRangeInclusive) {
    return makeRange1(argCount, args, true);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeRangeExclusive) {
    return makeRange1(argCount, args, false);
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
