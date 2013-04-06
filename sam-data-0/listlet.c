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
 * Helper definitions
 */

/** The empty listlet, lazily initialized. */
static zvalue theEmptyListlet = NULL;

/**
 * Allocates a listlet of the given size.
 */
static zvalue allocListlet(zint size) {
    return samAllocValue(SAM_LISTLET, size, size * sizeof(zvalue));
}

/**
 * Gets the elements array from a listlet.
 */
static zvalue *listletElems(zvalue listlet) {
    samAssertListlet(listlet);

    return ((SamListlet *) listlet)->elems;
}


/*
 * Intra-library API implementation
 */

/** Documented in `impl.h`. */
zcomparison samListletCompare(zvalue v1, zvalue v2) {
    zvalue *e1 = listletElems(v1);
    zvalue *e2 = listletElems(v2);
    zint sz1 = samSize(v1);
    zint sz2 = samSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i], e2[i]);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    return (sz1 < sz2) ? SAM_IS_LESS : SAM_IS_MORE;
}


/*
 * API Implementation
 */

/** Documented in API header. */
zvalue samListletGet(zvalue listlet, zint n) {
    samAssertNth(listlet, n);
    return listletElems(listlet)[n];
}

/** Documented in API header. */
zvalue samListletEmpty(void) {
    if (theEmptyListlet == NULL) {
        theEmptyListlet = allocListlet(0);
    }

    return theEmptyListlet;
}

/** Documented in API header. */
zvalue samListletAppend(zvalue listlet, zvalue value) {
    samAssertListlet(listlet);

    zint oldSize = listlet->size;
    zint size = oldSize + 1;
    zvalue result = allocListlet(size);

    memcpy(listletElems(result), listletElems(listlet),
           oldSize * sizeof(zvalue));
    listletElems(result)[oldSize] = value;

    return result;
}

/** Documented in API header. */
zvalue samListletFromUtf8String(const zbyte *string, zint stringBytes) {
    zint decodedSize = samUtf8DecodeStringSize(string, stringBytes);
    zvalue result = allocListlet(decodedSize);

    samUtf8DecodeStringToValues(string, stringBytes, listletElems(result));
    return result;
}

/** Documented in API header. */
zvalue samListletFromAsciiString(const zbyte *string) {
    zint size = strlen((const void *) string);
    zvalue result = allocListlet(size);
    zvalue *elems = listletElems(result);

    for (zint i = 0; i < size; i++) {
        zbyte one = string[i];
        if (one >= 0x80) {
            samDie("Invalid ASCII byte: 0x%02x", one);
        }
        elems[i] = samIntletFromInt(one);
    }

    return result;
}
