/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "util.h"


/*
 * Helper functions
 */

enum {
    BITS_PER_WORD = 64
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

    return samAllocValue(SAM_INTLET, wordCount * BITS_PER_WORD, wordCount);
}

/**
 * Gets the elements array from an intlet.
 */
static zint *intletElems(zvalue intlet) {
    samAssertIntlet(intlet);

    return ((SamIntlet *) intlet)->elems;
}


/*
 * Intra-library API implementation
 */

/** Documented in `impl.h`. */
zcomparison samIntletCompare(zvalue v1, zvalue v2) {
    bool neg1 = samIntletSign(v1);
    bool neg2 = samIntletSign(v2);

    if (neg1 != neg2) {
        return neg1 ? SAM_IS_LESS : SAM_IS_MORE;
    }

    // At this point, we know the two numbers are the same sign,
    // which makes it okay to do unsigned comparison (because, in
    // particular, the unsigned interpretations of two negative numbers
    // sort the same as the corresponding negative numbers).

    zint sz1 = samSize(v1);
    zint sz2 = samSize(v2);
    zint sz = (sz1 > sz2) ? sz1 : sz2;

    for (zint i = sz - 1; i >= 0; i--) {
        zint n1 = samIntletGetInt(v1, i);
        zint n2 = samIntletGetInt(v2, i);
        if (n1 < n2) {
            return SAM_IS_LESS;
        } else if (n1 > n2) {
            return SAM_IS_MORE;
        }
    }

    return SAM_IS_EQUAL;
}


/*
 * API implementation
 */

/** Documented in API header. */
bool samIntletGetBit(zvalue intlet, zint n) {
    zint word = wordIndex(n);
    zint bit = bitIndex(n);
    zint elem = samIntletGetInt(intlet, word);

    return (bool) ((elem >> bit) & 1);
}

/** Documented in API header. */
zint samIntletGetByte(zvalue intlet, zint n) {
    n *= 8; // 8 bits per byte.

    zint word = wordIndex(n);
    zint bit = bitIndex(n);
    zint elem = samIntletGetInt(intlet, word);

    return (elem >> bit) & 0xff;
}

/** Documented in API header. */
zint samIntletGetInt(zvalue intlet, zint n) {
    samAssertIntlet(intlet);

    zint wordSize = wordIndex(samSize(intlet));
    if (n < wordSize) {
        return intletElems(intlet)[n];
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
    zvalue result = allocIntlet(1);

    intletElems(result)[0] = value;
    return result;
}
