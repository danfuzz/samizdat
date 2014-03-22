/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "type/List.h"
#include "type/String.h"
#include "type/Type.h"
#include "io.h"
#include "util.h"

#include <string.h>


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue ioSplitAbsolutePath(zvalue flatPath) {
    zint sz = utf8SizeFromString(flatPath);
    char flat[sz + 1]; // `+1` for the null byte.

    if (sz == 0) {
        die("Cannot convert empty `flatPath`.");
    }

    utf8FromString(sz + 1, flat, flatPath);

    if (flat[0] != '/') {
        die("`flatPath` is not absolute.");
    }

    zint resultSz = 0;
    for (zint i = 0; i < sz; i++) {
        if (flat[i] == '/') {
            resultSz++;
        }
    }

    zvalue result[resultSz];
    for (zint i = 0, at = 0; flat[i]; /*empty*/) {
        i++; // Skip the '/'.

        zint end = i + 1;
        while (flat[end] && (flat[end] != '/')) {
            end++;
        }

        result[at] = stringFromUtf8(end - i, &flat[i]);
        i = end;
        at++;
    }

    return listFromArray(resultSz, result);
}
