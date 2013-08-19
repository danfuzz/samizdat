/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Safe integer functions
 */

#include "util.h"


/*
 * Private Definitions
 */

// Documented in header.
extern bool zintCanDivide(zint x, zint y);


/*
 * Exported Definitions
 */

// All documented in header.
extern bool zcharFromZint(zchar *result, zint value);
extern bool zintAbs(zint *result, zint x);
extern bool zintAdd(zint *result, zint x, zint y);
extern bool zintAnd(zint *result, zint x, zint y);
extern bool zintBit(zint *result, zint x, zint y);
extern zint zintBitSize(zint value);
extern bool zintDiv(zint *result, zint x, zint y);
extern bool zintDivEu(zint *result, zint x, zint y);
extern bool zintMod(zint *result, zint x, zint y);
extern bool zintModEu(zint *result, zint x, zint y);
extern bool zintMul(zint *result, zint x, zint y);
extern bool zintNeg(zint *result, zint x);
extern bool zintNot(zint *result, zint x);
extern bool zintOr(zint *result, zint x, zint y);
extern bool zintSign(zint *result, zint x);

/* Documented in header. */
bool zintShl(zint *result, zint x, zint y) {
    zint res;

    if ((x == 0) || (y == 0)) {
        res = x;
    } else if (y > 0) {
        // Left shift (and `x` is non-zero). There's definite loss of
        // bits if `y` is more than the size of an int. With a potentially
        // in-range `y`, we have to check based on the sign of `x`, ensuring
        // that a positive `x` is small enough or a negative `x` is large
        // enough that the shift couldn't lose its top significant bit.
        if ((y >= ZINT_BITS) ||
            ((x > 0) && (x > (ZINT_MAX >> y))) ||
            ((x < 0) && (x < (ZINT_MIN >> y)))) {
            return false;
        }
        res = x << y;
    } else {
        // Right shift. It's always safe, but we have to behave specially
        // when `y <= -ZINT_BITS`, as C99 leaves it undefined when the
        // right-hand side is greater than or equal to the number of bits
        // in the type in question.
        //
        // Also, note that strictly speaking, this isn't portable for
        // negative numbers, as the C99 standard does not say what right shift
        // on negative numbers means. In practice, right shift on negative
        // numbers means what it usually means for twos-complement; the
        // weaseliness of the spec is apparently just to allow for the
        // possibility ones-complement integer implementations.
        if (y <= -ZINT_BITS) {
            res = x >> (ZINT_BITS - 1);
        } else {
            res = x >> -y;
        }
    }

    if (result != NULL) {
        *result = res;
    }

    return true;
}

extern bool zintShr(zint *result, zint x, zint y);

/* Documented in header. */
bool zintSub(zint *result, zint x, zint y) {
    // Note: This can't be written as `zintAdd(x, -y)`, because of the
    // asymmetry of twos-complement integers. That is, that would fail if
    // `y == ZINT_MIN`.

    // Overflow can only happen when the two arguments are of opposite sign.
    // The two halves of the test here are equivalent, with each part
    // being a test of the limit of possible `x`s given `y`.
    if (((y < 0) && (x > (ZINT_MAX + y))) ||
        ((y > 0) && (x < (ZINT_MIN + y)))) {
        return false;
    }

    if (result != NULL) {
        *result = x - y;
    }

    return true;
}

extern bool zintXor(zint *result, zint x, zint y);
