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
 * Calls `datStringNth()`, converting the result into a proper zvalue.
 */
static zvalue valueFromStringNth(zvalue string, zint n) {
    zint ch = datStringNth(string, n);

    return (ch < 0) ? NULL : constStringFromZchar(ch);
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(charFromInt) {
    requireExactly(argCount, 1);
    return constStringFromZchar(datZcharFromInt(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intFromChar) {
    requireExactly(argCount, 1);

    zvalue string = args[0];
    datAssertStringSize1(string);

    return constIntFromZint(datStringNth(string, 0));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringAdd) {
    switch (argCount) {
        case 0: {
            return STR_EMPTY;
        }
        case 1: {
            datAssertString(args[0]);
            return args[0];
        }
        case 2: {
            return datStringAdd(args[0], args[1]);
        }
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        size += datSize(args[i]);
    }

    zchar chars[size];

    for (zint i = 0, at = 0; i < argCount; i++) {
        datZcharsFromString(&chars[at], args[i]);
        at += datSize(args[i]);
    }

    return datStringFromZchars(size, chars);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringNth) {
    requireExactly(argCount, 2);
    return doNth(valueFromStringNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringSlice) {
    requireAtLeast(argCount, 2);

    zvalue string = args[0];
    zint startIndex = datZintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? datZintFromInt(args[2]) : datSize(string);

    return datStringSlice(string, startIndex, endIndex);
}
