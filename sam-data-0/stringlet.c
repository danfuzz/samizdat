/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "unicode.h"
#include "util.h"

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
zvalue samStringletFromUtf8String(const zbyte *string, zint stringBytes) {
    zint decodedSize = samUtf8DecodeStringSize(string, stringBytes);
    zvalue result = samAllocListlet(decodedSize);

    samUtf8DecodeStringToValues(string, stringBytes, samListletElems(result));
    return result;
}

/** Documented in API header. */
zvalue samStringletFromAsciiString(const zbyte *string) {
    zint size = strlen((const void *) string);
    zvalue result = samAllocListlet(size);
    zvalue *elems = samListletElems(result);

    for (zint i = 0; i < size; i++) {
        zbyte one = string[i];
        if (one >= 0x80) {
            samDie("Invalid ASCII byte: %#02x", one);
        }
        elems[i] = samIntletFromInt(one);
    }

    return result;
}
