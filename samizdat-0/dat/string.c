/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <string.h>


/*
 * Helper definitions
 */

/**
 * Allocates a string of the given size.
 */
static zvalue allocString(zint size) {
    zvalue result =
        datAllocValue(DAT_String, sizeof(DatString) + size * sizeof(zchar));

    ((DatString *) result)->size = size;
    return result;
}

/**
 * Gets the size of a string.
 */
static zint stringSizeOf(zvalue string) {
    return ((DatString *) string)->size;
}

/**
 * Gets the array of `zvalue` elements from a list.
 */
static zchar *stringElems(zvalue string) {
    return ((DatString *) string)->elems;
}


/*
 * Module functions
 */

/* Documented in header. */
bool datStringEq(zvalue v1, zvalue v2) {
    zchar *e1 = stringElems(v1);
    zchar *e2 = stringElems(v2);
    zint size = stringSizeOf(v1);

    for (zint i = 0; i < size; i++) {
        if (e1[i] != e2[i]) {
            return false;
        }
    }

    return true;
}

/* Documented in header. */
zorder datStringOrder(zvalue v1, zvalue v2) {
    zchar *e1 = stringElems(v1);
    zchar *e2 = stringElems(v2);
    zint sz1 = stringSizeOf(v1);
    zint sz2 = stringSizeOf(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zchar c1 = e1[i];
        zchar c2 = e2[i];

        if (c1 < c2) {
            return ZLESS;
        } else if (c1 > c2) {
            return ZMORE;
        }
    }

    if (sz1 == sz2) {
        return ZSAME;
    }

    return (sz1 < sz2) ? ZLESS : ZMORE;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datStringAdd(zvalue str1, zvalue str2) {
    datAssertString(str1);
    datAssertString(str2);

    zint size1 = stringSizeOf(str1);
    zint size2 = stringSizeOf(str2);

    if (size1 == 0) {
        return str2;
    } else if (size2 == 0) {
        return str1;
    }

    zvalue result = allocString(size1 + size2);

    memcpy(stringElems(result), stringElems(str1),
           size1 * sizeof(zchar));
    memcpy(stringElems(result) + size1, stringElems(str2),
           size2 * sizeof(zchar));
    return result;
}

/* Documented in header. */
zvalue datStringFromZchars(zint size, const zchar *chars) {
    zvalue result = allocString(size);

    memcpy(stringElems(result), chars, size * sizeof(zchar));
    return result;
}

/* Documented in header. */
zvalue datStringFromUtf8(zint stringBytes, const char *string) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = utf8DecodeStringSize(stringBytes, string);
    zvalue result = allocString(decodedSize);

    utf8DecodeCharsFromString(stringElems(result), stringBytes, string);
    return result;
}

/* Documented in header. */
zint datStringNth(zvalue string, zint n) {
    datAssertString(string);
    return datHasNth(string, n) ? stringElems(string)[n] : (zint) -1;
}

/* Documented in header. */
zvalue datStringSlice(zvalue string, zint start, zint end) {
    datAssertString(string);
    datAssertSliceRange(string, start, end);

    return datStringFromZchars(end - start, &stringElems(string)[start]);
}

/* Documented in header. */
void datUtf8FromString(zint resultSize, char *result, zvalue string) {
    datAssertString(string);

    zint size = stringSizeOf(string);
    zchar *elems = stringElems(string);
    char *out = result;

    for (zint i = 0; i < size; i++) {
        out = utf8EncodeOne(out, elems[i]);
    }

    *out = '\0';
    out++;

    if ((out - result) > resultSize) {
        die("Buffer too small for utf8-encoded string.");
    }
}

/* Documented in header. */
zint datUtf8SizeFromString(zvalue string) {
    datAssertString(string);

    zint size = stringSizeOf(string);
    zchar *elems = stringElems(string);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        result += (utf8EncodeOne(NULL, elems[i]) - (char *) NULL);
    }

    return result;
}

/* Documented in header. */
void datZcharsFromString(zchar *result, zvalue string) {
    datAssertString(string);

    memcpy(result, stringElems(string), stringSizeOf(string) * sizeof(zchar));
}


/*
 * Type binding
 */

/* Documented in header. */
static void stringGcMark(zvalue string) {
    // Nothing to do here.
}

/* Documented in header. */
static DatType INFO_String = {
    .id = DAT_STRING,
    .name = "String",
    .sizeOf = stringSizeOf,
    .gcMark = stringGcMark,
    .gcFree = NULL
};
ztype DAT_String = &INFO_String;
