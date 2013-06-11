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
PRIM_IMPL(intFromString) {
    requireExactly(argCount, 1);

    zvalue string = args[0];
    datAssertStringSize1(string);

    return constIntFromZint(datStringNth(string, 0));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringFromInt) {
    requireExactly(argCount, 1);
    return constStringFromZchar(datZcharFromInt(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringAdd) {
    zvalue result = STR_EMPTY;

    for (zint i = 0; i < argCount; i++) {
        result = datStringAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringNth) {
    return doNth(valueFromStringNth, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringSlice) {
    requireAtLeast(argCount, 2);

    zvalue string = args[0];
    zint startIndex = datZintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? datZintFromInt(args[2]) : datSize(string);

    return datStringSlice(string, startIndex, endIndex);
}
