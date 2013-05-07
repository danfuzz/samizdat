/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"


/*
 * Exported functions
 */

/* Documented in header. */
const char *utilStringEnd(zint stringBytes, const char *string) {
    if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    const char *result = string + stringBytes;

    if (result < string) {
        die("Invalid string size (pointer wraparound): %p + %lld",
               string, stringBytes);
    }

    return result;
}
