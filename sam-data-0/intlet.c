/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"

/** Documented in API header. */
bool samIntletGetBit(zvalue intlet, zint n) {
    samAssertIntlet(intlet);

    zint word = n / 64;
    zint bitInWord = n % 64;
    zint elem = samIntletGetInt(intlet, word);

    return (bool) ((elem >> bitInWord) & 1);
}

/** Documented in API header. */
zint samIntletGetByte(zvalue intlet, zint n) {
    samAssertIntlet(intlet);

    zint word = n / 4;
    zint byteInWord = n % 4;
    zint elem = samIntletGetInt(intlet, word);

    return (elem >> (byteInWord * 8)) & 0xff;
}

/** Documented in API header. */
zint samIntletGetInt(zvalue intlet, zint n) {
    samAssertIntlet(intlet);

    zint wordSize = (samSize(intlet) + 63) / 64;
    if (n < wordSize) {
        return ((SamIntlet *) intlet)->elems[n];
    } else {
        return samIntletSign(intlet) ? (zint) -1 : 0;
    }
}

/** Documented in API header. */
bool samIntletSign(zvalue intlet) {
    zint size = samSize(intlet);
    return samIntletGetBit(intlet, size - 1);
}

/** Documented in API header. */
zvalue samIntletFromInt(zint value) {
    zvalue result = samAllocValue(SAM_INTLET, 1, sizeof(zint));

    ((SamIntlet *) result)->elems[0] = value;
    return result;
}
