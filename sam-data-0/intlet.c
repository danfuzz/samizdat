/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"

/** Documented in API header. */
zint samIntletGetBit(zvalue intlet, zint n) {
    samAssertIntlet(intlet);
    samAssertNth(intlet, n);

    zint word = n / 64;
    zint bitInWord = n % 64;
    zint elem = samIntletGetInt(intlet, word);

    return (elem >> bitInWord) & 1;
}

/** Documented in API header. */
zint samIntletGetByte(zvalue intlet, zint n) {
    samAssertIntlet(intlet);
    samAssertNth(intlet, n * 8);

    zint word = n / 4;
    zint byteInWord = n % 4;
    zint elem = samIntletGetInt(intlet, word);

    return (elem >> (byteInWord * 8)) & 0xff;
}

/** Documented in API header. */
zint samIntletGetInt(zvalue intlet, zint n) {
    samAssertIntlet(intlet);
    samAssertNth(intlet, n * 64);

    return ((SamIntlet *) intlet)->elems[n];
}

/** Documented in API header. */
zvalue samIntletFromInt(zint value) {
    zvalue result = samAllocValue(SAM_INTLET, 1, sizeof(zint));

    ((SamIntlet *) result)->elems[0] = value;
    return result;
}
