/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"


/*
 * Helper definitions
 */

enum {
    /** Ints are restricted to being in the range of `int32_t`. */
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
 * Gets the value of the given integer as a `zint`. Doesn't do any
 * type checking.
 */
static zint intValue(zvalue integer) {
    return ((DatInt *) integer)->value;
}


/*
 * Module functions
 */

/* Documented in header. */
bool datIntEq(zvalue v1, zvalue v2) {
    return intValue(v1) == intValue(v2);
}

/* Documented in header. */
zorder datIntOrder(zvalue v1, zvalue v2) {
    zint int1 = intValue(v1);
    zint int2 = intValue(v2);

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
zchar datCharFromInt(zvalue integer) {
    zint value = datZintFromInt(integer);

    if ((value < 0) || (value >= 0x100000000)) {
        die("Invalid integer value for character: %lld", value);
    }

    return (zchar) value;
}

/* Documented in header. */
bool datZintGetBit(zint value, zint n) {
    if (n < 0) {
        die("Attempt to access negative bit index: %lld", n);
    } else if (n >= MAX_BITS) {
        n = MAX_BITS - 1;
    }

    return (bool) ((value >> n) & 1);
}

/* Documented in header. */
bool datIntGetBit(zvalue integer, zint n) {
    datAssertInt(integer);
    return datZintGetBit(intValue(integer), n);
}

/* Documented in header. */
bool datIntSign(zvalue integer) {
    return datIntGetBit(integer, MAX_BITS - 1);
}

/* Documented in header. */
zvalue datIntFromZint(zint value) {
    zint size = bitSize(value);
    zvalue result = datAllocValue(DAT_INT, size, sizeof(int32_t));

    if (size > MAX_BITS) {
        die("Value too large to fit into integer: %lld", value);
    }

    ((DatInt *) result)->value = (int32_t) value;
    return result;
}

/* Documented in header. */
zint datZintFromInt(zvalue integer) {
    datAssertInt(integer);
    return intValue(integer);
}
