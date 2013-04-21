/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Helper definitions
 */

enum {
    /** Intlets are restricted to being in the range of `int32_t`. */
    MAX_BITS = 32
};

/**
 * Gets the bit size (highest-order significant bit number, plus one)
 * of the given `zint`.
 */
static zint bitSize(zint value) {
    if (value < 0) {
        value = ~value;
    }

    // "Binary-search" style implementation. Many compilers have a
    // built-in "count leading zeroes" function, but we're aiming
    // for portability here.

    zint result = 1; // +1 in that we want size, not zero-based bit number.
    uint64_t uv = (uint64_t) value; // Use `uint` to account for `-MAX_ZINT`.

    if (uv >= ((zint) 1 << 32)) { result += 32; uv >>= 32; }
    if (uv >= ((zint) 1 << 16)) { result += 16; uv >>= 16; }
    if (uv >= ((zint) 1 << 8))  { result +=  8; uv >>=  8; }
    if (uv >= ((zint) 1 << 4))  { result +=  4; uv >>=  4; }
    if (uv >= ((zint) 1 << 2))  { result +=  2; uv >>=  2; }
    if (uv >= ((zint) 1 << 1))  { result +=  1; uv >>=  1; }
    return result + uv;
}

/**
 * Gets the value of the given intlet as a `zint`. Doesn't do any
 * type checking.
 */
static zint intletValue(zvalue intlet) {
    return ((DatIntlet *) intlet)->value;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datIntletOrder(zvalue v1, zvalue v2) {
    zint int1 = intletValue(v1);
    zint int2 = intletValue(v2);

    if (int1 < int2) {
        return ZLESS;
    } else if (int1 > int2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
bool datIntletGetBit(zvalue intlet, zint n) {
    datAssertIntlet(intlet);

    if (n >= MAX_BITS) {
        n = MAX_BITS - 1;
    }

    return (bool) ((intletValue(intlet) >> n) & 1);
}

/* Documented in header. */
bool datIntletSign(zvalue intlet) {
    return datIntletGetBit(intlet, MAX_BITS - 1);
}

/* Documented in header. */
zvalue datIntletFromInt(zint value) {
    zint size = bitSize(value);
    zvalue result = datAllocValue(DAT_INTLET, size, sizeof(int32_t));

    if (size > MAX_BITS) {
        die("Value too large to fit into intlet: %lld", value);
    }

    ((DatIntlet *) result)->value = (int32_t) value;
    return result;
}

/* Documented in header. */
zint datIntFromIntlet(zvalue intlet) {
    datAssertIntlet(intlet);
    return intletValue(intlet);
}
