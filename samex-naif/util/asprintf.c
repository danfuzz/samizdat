// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


// Needed for `vasprintf` when using glibc.
#define _GNU_SOURCE

#include <stdarg.h>
#include <stdio.h>

#include "util.h"

//
// Exported Definitions
//

// Documented in header.
int utilAsprintf(char **ret, const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    zint result = vasprintf(ret, format, rest);
    va_end(rest);

    // Note: Should `utilFree()` ever be something more than a pass-through
    // to `free()`, then it will become necessary to re-allocate the result.

    return result;
}
