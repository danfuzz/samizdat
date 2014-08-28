// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <string.h>

#include "util.h"

//
// Exported Definitions
//

// Documented in header.
void arrayFromZstring(zchar *result, const zstring *string) {
    utilCpy(zchar, result, string->chars, string->size);
}

// Documented in header.
char *utf8DupFromZstring(const zstring *string) {
    zint size = utf8SizeFromZstring(string) + 1;  // `+1` for the final `\0`.
    char *result = utilAlloc(size);

    utf8FromZstring(size, result, string);
    return result;
}

// Documented in header.
zint utf8FromZstring(zint resultSize, char *result, const zstring *string) {
    char *out = result;

    for (zint i = 0; i < string->size; i++) {
        out = utf8EncodeOne(out, string->chars[i]);
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
zint utf8SizeFromZstring(const zstring *string) {
    zint result = 0;

    for (zint i = 0; i < string->size; i++) {
        result += (utf8EncodeOne(NULL, string->chars[i]) - (char *) NULL);
    }

    return result;
}

// Documented in header.
zint zstringAllocSize(zint size) {
    return sizeof(zstring) + (size * sizeof(zchar));
}

// Documented in header.
zint zstringAllocSizeFromUtf8(zint utfBytes, const char *utf) {
    return zstringAllocSize(utf8DecodeStringSize(utfBytes, utf));
}

// Documented in header.
zstring *zstringDupFromUtf8(zint utfBytes, const char *utf) {
    zint size = utf8DecodeStringSize(utfBytes, utf);
    zstring *result = utilAlloc(zstringAllocSize(size));

    result->size = size;
    utf8DecodeCharsFromString(result->chars, utfBytes, utf);
}

// Documented in header.
bool zstringEq(const zstring *string1, const zstring *string2) {
    if (string1 == string2) {
        return true;
    }

    zint size = string1->size;

    if (size != string2->size) {
        return false;
    }

    return memcmp(string1->chars, string2->chars, size * sizeof(zchar)) == 0;
}

// Documented in header.
void zstringFromArray(zstring *result, zint size, const zchar *chars) {
    result->size = size;
    utilCpy(zchar, result->chars, chars, size);
}

// Documented in header.
void zstringFromUtf8(zstring *result, zint utfBytes, const char *utf) {
    zint size = utf8DecodeStringSize(utfBytes, utf);

    result->size = size;
    utf8DecodeCharsFromString(result->chars, utfBytes, utf);
}

// Documented in header.
zorder zstringOrder(const zstring *string1, const zstring *string2) {
    if (string1 == string2) {
        return ZSAME;
    }

    zint size1 = string1->size;
    zint size2 = string2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zchar c1 = string1->chars[i];
        zchar c2 = string2->chars[i];

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
