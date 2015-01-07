// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <string.h>

#include "type/List.h"
#include "type/String.h"
#include "type/Class.h"
#include "io.h"
#include "util.h"


//
// Private Definitions
//

/**
 * Common code for checking paths.
 */
void checkPath0(zvalue path, bool isAbsolute) {
    if (!typeAccepts(CLS_String, path)) {
        die("Invalid path: not a string");
    }

    zint sz = utf8SizeFromString(path);
    char str[sz + 1];  // `+1` for the null byte.

    if (sz == 0) {
        die("Invalid path: empty string");
    }

    utf8FromString(sz + 1, str, path);

    if (isAbsolute && (str[0] != '/')) {
        die("Invalid path: not absolute");
    }

    for (zint i = 0; i < sz; i++) {
        if (str[i] == '\0') {
            die("Invalid path: contains `\\0` character");
        }
    }
}

//
// Exported Definitions
//

// Documented in header.
void ioCheckAbsolutePath(zvalue path) {
    checkPath0(path, true);
}

// Documented in header.
void ioCheckPath(zvalue path) {
    checkPath0(path, false);
}
