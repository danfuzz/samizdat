/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper functions
 */

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zvalue`s. The buffer must be sufficiently
 * large to hold the result of decoding. Each of the decoded values
 * is an intlet.
 */
static void decodeStringToValues(const char *string, zint stringBytes,
                                 zvalue *result) {
    const char *stringEnd = strGetEnd(string, stringBytes);
    zint one = 0;

    while (string < stringEnd) {
        string = utf8DecodeOne(string, stringEnd - string, &one);
        *result = datIntletFromInt(one);
        result++;
    }
}

/**
 * Gets the UTF-8 encoded size of the given stringlet, in bytes.
 */
static zint utf8Size(zvalue stringlet) {
    datAssertStringlet(stringlet);

    zint size = datSize(stringlet);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        zint ch = datListletGetInt(stringlet, i);
        result += (utf8EncodeOne(NULL, ch) - (char *) NULL);
    }

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertStringlet(zvalue value) {
    datAssertListlet(value);

    zint size = datSize(value);
    for (zint i = 0; i < size; i++) {
        zvalue one = datListletGet(value, i);
        zint v = datIntletToInt(one);
        uniAssertValid(v);
    }
}

/* Documented in header. */
zvalue datStringletFromUtf8String(const char *string, zint stringBytes) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = utf8DecodeStringSize(string, stringBytes);
    zvalue result = datAllocListlet(decodedSize);

    decodeStringToValues(string, stringBytes, datListletElems(result));
    return result;
}

/* Documented in header. */
const char *datStringletEncodeUtf8(zvalue stringlet, zint *resultSize) {
    zint size = datSize(stringlet);
    zint utfSize = utf8Size(stringlet);
    char *result = zalloc(utfSize + 1);
    char *out = result;

    for (zint i = 0; i < size; i++) {
        out = utf8EncodeOne(out, datListletGetInt(stringlet, i));
    }

    if (resultSize != NULL) {
        *resultSize = utfSize;
    }

    *out = '\0';
    return result;
}
