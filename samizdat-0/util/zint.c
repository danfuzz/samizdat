/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `zint` Utilities
 *
 * On the topic of arithmetic operations: C compilers are allowed to
 * optimize overflowing operations in surprising ways. Therefore, the usual
 * recommendation for portable code is to detect overflow by inspection of
 * operands, not results. The code here is informed by the CERT "AIR"
 * recommendations. Also, note that the remainder implementations here
 * rely on the C99 definition for the `%` operator (that is, sign matches
 * the left-hand side).
 *
 * References:
 *
 * * [As-If Infinitely Ranged Integer Model, Second
 *   Edition](http://www.cert.org/archive/pdf/10tn008.pdf)
 *
 * * [Division and Modulus for Computer
 *   Scientists](http://legacy.cs.uu.nl/daan/download/papers/divmodnote.pdf)
 *
 * * [Modulo operation
 *   (Wikipedia)](http://en.wikipedia.org/wiki/Modulo_operation)
 *
 * * [Safe IOP Library](https://code.google.com/p/safe-iop/)
 */

#include "util.h"


/*
 * Private Definitions
 */

/**
 * Common check for all divide and remainder functions.
 */
static bool canDivide(zint v1, zint v2) {
    if (v2 == 0) {
        // Divide by zero.
        return false;
    }

    if ((v1 == ZINT_MIN) && (v2 == -1)) {
        // Overflow: `-ZINT_MIN` is not representable as a `zint`.
        return false;
    }

    return true;
}


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
bool zintAdd(zint *result, zint v1, zint v2) {
    // If the signs are opposite or either argument is zero, then overflow
    // is impossible. The two clauses here are for the same-sign-and-not-zero
    // cases.
    if ((v1 > 0) && (v2 > 0)) {
        // Both arguments are positive. The largest that `v1` can be is the
        // difference between `v2` and the largest representable value.
        if (v1 > (ZINT_MAX - v2)) {
            return false;
        }
    } else if ((v1 < 0) && (v2 < 0)) {
        // Both arguments are negative. The smallest that `v1` can be is the
        // difference between `v2` and the smallest representable value.
        if (v1 < (ZINT_MIN - v2)) {
            return false;
        }
    }

    if (result != NULL) {
        *result = v1 + v2;
    }

    return true;
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
    uint64_t uv = (uint64_t) value; // Use `uint` to account for `-ZINT_MAX`.

    if (uv >= ((zint) 1 << 32)) { result += 32; uv >>= 32; }
    if (uv >= ((zint) 1 << 16)) { result += 16; uv >>= 16; }
    if (uv >= ((zint) 1 << 8))  { result +=  8; uv >>=  8; }
    if (uv >= ((zint) 1 << 4))  { result +=  4; uv >>=  4; }
    if (uv >= ((zint) 1 << 2))  { result +=  2; uv >>=  2; }
    if (uv >= ((zint) 1 << 1))  { result +=  1; uv >>=  1; }
    return result + uv;
}

/* Documented in header. */
bool zintDiv(zint *result, zint v1, zint v2) {
    if (!canDivide(v1, v2)) {
        return false;
    }

    if (result != NULL) {
        *result = v1 / v2;
    }

    return true;
}

/* Documented in header. */
bool zintDivEu(zint *result, zint v1, zint v2) {
    if (!canDivide(v1, v2)) {
        return false;
    }

    if (result != NULL) {
        zint quo = v1 / v2;
        zint rem = v1 % v2;
        if (rem < 0) {
            if (v2 > 0) { quo--; }
            else        { quo++; }
        }
        *result = quo;
    }

    return true;
}

/* Documented in header. */
bool zintGetBit(zint *result, zint v1, zint v2) {
    if (v2 < 0) {
        return false;
    }

    if (result != NULL) {
        if (v2 >= ZINT_BITS) {
            v2 = ZINT_BITS - 1;
        }

        *result = (v1 >> v2) & 1;
    }

    return true;
}

/* Documented in header. */
bool zintMod(zint *result, zint v1, zint v2) {
    if (!canDivide(v1, v2)) {
        return false;
    }

    if (result != NULL) {
        *result = v1 % v2;
    }

    return true;
}

/* Documented in header. */
bool zintModEu(zint *result, zint v1, zint v2) {
    if (!canDivide(v1, v2)) {
        return false;
    }

    if (result != NULL) {
        zint rem = v1 % v2;
        if (rem < 0) {
            if (v2 > 0) { rem += v2; }
            else        { rem -= v2; }
        }
        *result = rem;
    }

    return true;
}

/* Documented in header. */
bool zintMul(zint *result, zint v1, zint v2) {
    // This is broken down by sign of the arguments, with zeros getting
    // an easy pass-through.

    if (v1 > 0) {
        if (v2 > 0) {
            // Both arguments are positive.
            if (v1 > (ZINT_MAX / v2)) {
                return false;
            }
        } else if (v2 < 0) {
            // `v1` is positive, and `v2` is negative.
            if (v2 < (ZINT_MIN / v1)) {
                return false;
            }
        }
    } else if (v1 < 0) {
        if (v2 > 0) {
            // `v1` is negative, and `v2` is positive.
            if (v1 < (ZINT_MIN / v2)) {
                return false;
            }
        } else if (v2 < 0) {
            // Both arguments are negative.
            if (v2 < (ZINT_MAX / v1)) {
                return false;
            }
        }
    }

    if (result != NULL) {
        *result = v1 * v2;
    }

    return true;
}

/* Documented in header. */
bool zintShl(zint *result, zint v1, zint v2) {
    zint res;

    if (v2 > 0) {
        // Left shift. If `v1` isn't `0`, then there's a possibility of
        // loss of bits. In particular, the most that `v1` can be shifted by
        // is `ZINT_BITS - N - 1`, where `N` is the number of significant
        // bits in `v1` (including the sign).
        if ((v1 != 0) && (v2 >= (ZINT_BITS - zintBitSize(v1)))) {
            return false;
        }
        res = v1 << v2;
    } else if (v2 < 0) {
        // Right shift. It's always safe, but we have to behave specially
        // when `v2 <= -ZINT_BITS`, as C99 leaves it undefined when the
        // right-hand side is greater than the number of bits in the type
        // in question.
        //
        // Also, note that strictly speaking, this isn't portable for
        // negative numbers, as the C99 standard does not say what right shift
        // on negative numbers means. In practice, right shift on negative
        // numbers means what it usually means for twos-complement; the
        // weaseliness of the spec is apparently just to allow for the
        // possibility ones-complement integer implementations.
        if (v2 <= -ZINT_BITS) {
            res = v1 >> (ZINT_BITS - 1);
        } else {
            res = v1 >> -v2;
        }
    } else {
        res = v1;
    }

    if (result != NULL) {
        *result = res;
    }

    return true;
}

/* Documented in header. */
bool zintShr(zint *result, zint v1, zint v2) {
    // We just define this in terms of `zintShl`, but note the test to
    // deal with the possibility of passing `ZINT_MIN` for `v2`.
    return zintShl(result, v1, (v2 <= -ZINT_BITS) ? ZINT_BITS : -v2);
}

/* Documented in header. */
bool zintSub(zint *result, zint v1, zint v2) {
    // Note: This isn't equivalent to `zintAdd(v1, -v2)`, because of the
    // asymmetry of twos-complement integers. In particular,
    // `-ZINT_MIN == ZINT_MIN`. For example, `0 - ZINT_MIN` is an overflow.

    // Overflow can only happen when the two arguments are of opposite sign.
    // The two halves of the test here are equivalent, with each part
    // being a test of the limit of possible `v1`s given `v2`.
    if (((v2 <= 0) && (v1 > (ZINT_MAX + v2))) ||
        ((v2 > 0) && (v1 < (ZINT_MIN + v2)))) {
        return false;
    }

    if (result != NULL) {
        *result = v1 - v2;
    }

    return true;
}
