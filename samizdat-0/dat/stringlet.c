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
        *result = samIntletFromInt(one);
        result++;
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
void samAssertStringlet(zvalue value) {
    samAssertListlet(value);

    zint size = samSize(value);
    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(value, i);
        zint v = samIntletToInt(one);
        uniAssertValid(v);
    }
}

/* Documented in header. */
zvalue samStringletFromUtf8String(const char *string, zint stringBytes) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = utf8DecodeStringSize(string, stringBytes);
    zvalue result = samAllocListlet(decodedSize);

    decodeStringToValues(string, stringBytes, samListletElems(result));
    return result;
}

/* Documented in header. */
zint samStringletUtf8Size(zvalue stringlet) {
    samAssertStringlet(stringlet);

    zint size = samSize(stringlet);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        zint ch = samListletGetInt(stringlet, i);
        result += (utf8EncodeOne(NULL, ch) - (char *) NULL);
    }

    return result;
}

/* Documented in header. */
void samStringletEncodeUtf8(zvalue stringlet, char *utf8) {
    samAssertStringlet(stringlet);

    zint size = samSize(stringlet);

    for (zint i = 0; i < size; i++) {
        zint ch = samListletGetInt(stringlet, i);
        utf8 = utf8EncodeOne(utf8, ch);
    }
}
