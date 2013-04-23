/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

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

    return datIntletFromInt(datStringletNth(stringlet, 1));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromIntlet) {
    requireExactly(argCount, 1);
    return stringletFromChar(datCharFromIntlet(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletAdd) {
    requireExactly(argCount, 2);
    return datStringletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletNth) {
    requireRange(argCount, 2, 3);

    zvalue result = NULL;
    if (datTypeIs(args[1], DAT_INTLET)) {
        zint ch = datStringletNth(args[0], datIntFromIntlet(args[1]));
        if (ch >= 0) {
            result = stringletFromChar(ch);
        }
    }

    if (result == NULL) {
        return (argCount == 3) ? args[2] : NULL;
    } else {
        return result;
    }
}
