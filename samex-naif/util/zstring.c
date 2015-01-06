// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <string.h>

#include "util.h"

//
// Exported Definitions
//

// Documented in header.
void arrayFromZstring(zchar *result, const zstring string) {
    utilCpy(zchar, result, string.chars, string.size);
}

// Documented in header.
char *utf8DupFromZstring(zstring string) {
    zint size = utf8SizeFromZstring(string) + 1;  // `+1` for the final `\0`.
    char *result = utilAlloc(size);

    utf8FromZstring(size, result, string);
    return result;
}

// Documented in header.
zint utf8FromZstring(zint resultSize, char *result, zstring string) {
    char *out = result;

    for (zint i = 0; i < string.size; i++) {
        out = utf8EncodeOne(out, string.chars[i]);
    }

    *out = '\0';
    out++;

    zint finalSize = out - result;

    if (finalSize > resultSize) {
        die("Buffer too small for UTF-8-encoded string.");
    }

    return finalSize;
}

// Documented in header.
zint utf8SizeFromZstring(zstring string) {
    zint result = 0;

    for (zint i = 0; i < string.size; i++) {
        result += (utf8EncodeOne(NULL, string.chars[i]) - (char *) NULL);
    }

    return result;
}

// Documented in header.
bool zstringEq(zstring string1, zstring string2) {
    zint size = string1.size;

    if (size != string2.size) {
        return false;
    } else if (string1.chars == string2.chars) {
        return true;
    }

    return utilCmp(zchar, string1.chars, string2.chars, size) == 0;
}

// Documented in header.
zorder zstringOrder(zstring string1, zstring string2) {
    zint size1 = string1.size;
    zint size2 = string2.size;
    const zchar *chars1 = string1.chars;
    const zchar *chars2 = string2.chars;

    if ((size1 == size2) && (chars1 == chars2)) {
        return ZSAME;
    }

    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zchar c1 = chars1[i];
        zchar c2 = chars2[i];

        if (c1 < c2) {
            return ZLESS;
        } else if (c1 > c2) {
            return ZMORE;
        }
    }

    if (size1 == size2) {
        return ZSAME;
    }

    return (size1 < size2) ? ZLESS : ZMORE;
}
