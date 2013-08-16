/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `zint` Utilities
 */

#include "util.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
zchar zcharFromZint(zint value) {
    if ((value < 0) || (value > ZCHAR_MAX)) {
        die("Invalid zint value for zchar: %lld", value);
    }

    return (zchar) value;
}

/* Documented in header. */
zint zintBitSize(zint value) {
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

/* Documented in header. */
bool zintGetBit(zint value, zint n) {
    if (n < 0) {
        die("Attempt to access negative bit index: %lld", n);
    } else if (n >= ZINT_BITS) {
        n = ZINT_BITS - 1;
    }

    return (bool) ((value >> n) & 1);
}
