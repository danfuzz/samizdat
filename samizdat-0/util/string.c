/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "util.h"


/*
 * Exported functions
 */

/* Documented in header. */
const char *strGetEnd(const char *string, zint stringBytes) {
    if (stringBytes < 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    const char *result = string + stringBytes;

    if (result < string) {
        samDie("Invalid string size (pointer wraparound): %p + %lld",
               string, stringBytes);
    }

    return result;
}
