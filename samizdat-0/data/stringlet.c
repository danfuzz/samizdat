/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "unicode.h"

#include <stdlib.h>
#include <string.h>


/*
 * API Implementation
 */

/** Documented in API header. */
void samAssertStringlet(zvalue value) {
    samAssertListlet(value);

    zint size = samSize(value);
    for (zint i = 0; i < size; i++) {
        zvalue one = samListletGet(value, i);
        zint v = samIntletToInt(one);
        samAssertValidUnicode(v);
    }
}

/** Documented in API header. */
zvalue samStringletFromUtf8String(const char *string, zint stringBytes) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = samUtf8DecodeStringSize(string, stringBytes);
    zvalue result = samAllocListlet(decodedSize);

    samUtf8DecodeStringToValues(string, stringBytes, samListletElems(result));
    return result;
}

/** Documented in API header. */
zint samStringletUtf8Size(zvalue stringlet) {
    samAssertStringlet(stringlet);

    zint size = samSize(stringlet);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        zint ch = samListletGetInt(stringlet, i);
        result += (samUtf8EncodeOne(NULL, ch) - (char *) NULL);
    }

    return result;
}

/** Documented in API header. */
void samStringletEncodeUtf8(zvalue stringlet, char *utf8) {
    samAssertStringlet(stringlet);

    zint size = samSize(stringlet);

    for (zint i = 0; i < size; i++) {
        zint ch = samListletGetInt(stringlet, i);
        utf8 = samUtf8EncodeOne(utf8, ch);
    }
}
