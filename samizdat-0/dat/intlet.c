/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Helper functions
 */

enum {
    BITS_PER_BYTE = 8,
    BITS_PER_WORD = sizeof(zint) * BITS_PER_BYTE
};

/**
 * Gets the word index for the given bit index.
 */
static zint wordIndex(zint n) {
    return n / BITS_PER_WORD;
}

/**
 * Gets the bit-in-word index for the given bit index.
 */
static zint bitIndex(zint n) {
    return n % BITS_PER_WORD;
}

/**
 * Allocates an intlet of the given bit size.
 */
static zvalue allocIntlet(zint bitSize) {
    zint wordCount = wordIndex(bitSize + BITS_PER_WORD - 1);

    return datAllocValue(DAT_INTLET, wordCount * BITS_PER_WORD, wordCount);
}

/**
 * Gets the elements array from an intlet.
 */
static zint *intletElems(zvalue intlet) {
    datAssertIntlet(intlet);

    return ((DatIntlet *) intlet)->elems;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datIntletOrder(zvalue v1, zvalue v2) {
    bool neg1 = datIntletSign(v1);
    bool neg2 = datIntletSign(v2);

    if (neg1 != neg2) {
        return neg1 ? ZLESS : ZMORE;
    }

    // At this point, we know the two numbers are the same sign,
    // which makes it okay to do unsigned comparison (because, in
    // particular, the unsigned interpretations of two negative numbers
    // sort the same as the corresponding negative numbers).

    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
    zint sz = (sz1 > sz2) ? sz1 : sz2;

    for (zint i = sz - 1; i >= 0; i--) {
        zint n1 = datIntletGetInt(v1, i);
        zint n2 = datIntletGetInt(v2, i);
        if (n1 < n2) {
            return ZLESS;
        } else if (n1 > n2) {
            return ZMORE;
        }
    }

    return ZSAME;
}


/*
 * Exported functions
 */

/* Documented in header. */
bool datIntletGetBit(zvalue intlet, zint n) {
    zint word = wordIndex(n);
    zint bit = bitIndex(n);
    zint elem = datIntletGetInt(intlet, word);

    return (bool) ((elem >> bit) & 1);
}

/* Documented in header. */
zint datIntletGetByte(zvalue intlet, zint n) {
    n *= BITS_PER_BYTE;

    zint word = wordIndex(n);
    zint bit = bitIndex(n);
    zint elem = datIntletGetInt(intlet, word);

    return (elem >> bit) & 0xff;
}

/* Documented in header. */
zint datIntletGetInt(zvalue intlet, zint n) {
    datAssertIntlet(intlet);

    zint wordSize = wordIndex(datSize(intlet));
    if (n < wordSize) {
        return intletElems(intlet)[n];
    } else {
        return datIntletSign(intlet) ? (zint) -1 : 0;
    }
}

/* Documented in header. */
bool datIntletSign(zvalue intlet) {
    zint size = datSize(intlet);

    if (size == 0) {
        return false;
    }

    return datIntletGetBit(intlet, size - 1);
}

/* Documented in header. */
zvalue datIntletFromInt(zint value) {
    zvalue result = allocIntlet(1);

    intletElems(result)[0] = value;
    return result;
}

/* Documented in header. */
zint datIntletToInt(zvalue intlet) {
    datAssertIntlet(intlet);

    // Note: This relies on the intlet being in optimal form (no
    // superfluous high-order words).
    zint size = datSize(intlet);

    if (size > BITS_PER_WORD) {
        die("Out-of-range intlet.");
    }

    return (size == 0) ? 0 : intletElems(intlet)[0];
}
