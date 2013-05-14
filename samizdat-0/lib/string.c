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
 * Returns a single-character string.
  */
static zvalue stringFromChar(zchar ch) {
    return datStringFromChars(1, &ch);
}

/**
 * Calls `datStringNth()`, converting the result into a proper zvalue.
 */
static zvalue valueFromStringNth(zvalue string, zint n) {
    zint ch = datStringNth(string, n);

    return (ch < 0) ? NULL : stringFromChar(ch);
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(integerFromString) {
    requireExactly(argCount, 1);

    zvalue string = args[0];

    if (datSize(string) != 1) {
        die("Invalid use of string: size != 1");
    }

    return datIntFromZint(datStringNth(string, 0));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringFromInt) {
    requireExactly(argCount, 1);
    return stringFromChar(datCharFromInt(args[0]));
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
