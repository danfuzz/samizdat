/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"

/** Documented in API header. */
zint samIntletGetBit(zvalue intlet, zint n) {
    samAssertNth(intlet, n);

    zint word = n / 64;
    zint bitInWord = n % 64;
    zint value = samIntletGetInt(intlet, word);

    return (value >> bitInWord) & 1;
}

/** Documented in API header. */
zint samIntletGetByte(zvalue intlet, zint n) {
    samAssertNth(intlet, n * 8);

    zint word = n / 4;
    zint byteInWord = n % 4;
    zint value = samIntletGetInt(intlet, word);

    return (value >> (byteInWord * 8)) & 0xff;
}

/** Documented in API header. */
zint samIntletGetInt(zvalue intlet, zint n) {
    samAssertNth(intlet, n * 64);

    return ((SamIntlet *) intlet)->values[n];
}

/** Documented in API header. */
zvalue samIntletFromInt(zint value) {
    zvalue result = samAllocValue(sizeof(zint));

    ((SamIntlet *) result)->values[0] = value;
    return result;
}
