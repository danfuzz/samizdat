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
 * Allocates a stringlet of the given size.
 */
static zvalue allocStringlet(zint size) {
    return datAllocValue(DAT_STRINGLET, size, size * sizeof(zchar));
}

/**
 * Gets the array of `zvalue` elements from a listlet.
 */
static zchar *stringletElems(zvalue stringlet) {
    return ((DatStringlet *) stringlet)->elems;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datStringletOrder(zvalue v1, zvalue v2) {
    zchar *e1 = stringletElems(v1);
    zchar *e2 = stringletElems(v2);
    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
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
zint datStringletNth(zvalue stringlet, zint n) {
    datAssertStringlet(stringlet);
    return datHasNth(stringlet, n) ? stringletElems(stringlet)[n] : (zint) -1;
}

/* Documented in header. */
zvalue datStringletFromChars(zint size, const zchar *chars) {
    zvalue result = allocStringlet(size);

    memcpy(stringletElems(result), chars, size * sizeof(zchar));
    return result;
}

/* Documented in header. */
zvalue datStringletAdd(zvalue str1, zvalue str2) {
    datAssertStringlet(str1);
    datAssertStringlet(str2);

    zint size1 = datSize(str1);
    zint size2 = datSize(str2);
    zvalue result = allocStringlet(size1 + size2);

    memcpy(stringletElems(result), stringletElems(str1),
           size1 * sizeof(zchar));
    memcpy(stringletElems(result) + size1, stringletElems(str2),
           size2 * sizeof(zchar));
    return result;
}

/* Documented in header. */
zvalue datStringletFromUtf8(zint stringBytes, const char *string) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = utf8DecodeStringSize(stringBytes, string);
    zvalue result = allocStringlet(decodedSize);

    utf8DecodeCharsFromString(stringletElems(result), stringBytes, string);
    return result;
}

/* Documented in header. */
zint datUtf8SizeFromStringlet(zvalue stringlet) {
    datAssertStringlet(stringlet);

    zint size = datSize(stringlet);
    zchar *elems = stringletElems(stringlet);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        result += (utf8EncodeOne(NULL, elems[i]) - (char *) NULL);
    }

    return result;
}

/* Documented in header. */
const char *datUtf8FromStringlet(zint *resultSize, char *result,
                                 zvalue stringlet) {
    datAssertStringlet(stringlet);

    if (result == NULL) {
        zint utfSize = datUtf8SizeFromStringlet(stringlet);
        result = zalloc(utfSize + 1);
    }

    zint size = datSize(stringlet);
    zchar *elems = stringletElems(stringlet);
    char *out = result;

    for (zint i = 0; i < size; i++) {
        out = utf8EncodeOne(out, elems[i]);
    }

    if (resultSize != NULL) {
        *resultSize = out - result;
    }

    *out = '\0';
    return result;
}
