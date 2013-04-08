/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "unicode.h"
#include "util.h"

/*
 * Module functions
 */

/* Documented in header. */
void utf8DecodeStringToValues(const char *string, zint stringBytes,
                                 zvalue *result) {
    const char *stringEnd = strGetEnd(string, stringBytes);
    zint one = 0;

    while (string < stringEnd) {
        string = utf8DecodeOne(string, stringEnd - string, &one);
        *result = samIntletFromInt(one);
        result++;
    }
}
