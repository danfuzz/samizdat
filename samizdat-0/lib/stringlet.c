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
 * Returns a single-character stringlet.
  */
static zvalue stringletFromChar(zchar ch) {
    return datStringletFromChars(1, &ch);
}

/**
 * Calls `datStringletNth()`, converting the result into a proper zvalue.
 */
static zvalue valueFromStringletNth(zvalue stringlet, zint n) {
    zint ch = datStringletNth(stringlet, n);

    return (ch < 0) ? NULL : stringletFromChar(ch);
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intletFromStringlet) {
    requireExactly(argCount, 1);

    zvalue stringlet = args[0];

    if (datSize(stringlet) != 1) {
        die("Invalid use of stringlet: size != 1");
    }

    return datIntletFromInt(datStringletNth(stringlet, 0));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromIntlet) {
    requireExactly(argCount, 1);
    return stringletFromChar(datCharFromIntlet(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletAdd) {
    zvalue result = STR_EMPTY;

    for (zint i = 0; i < argCount; i++) {
        result = datStringletAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletNth) {
    return doNth(valueFromStringletNth, argCount, args);
}
