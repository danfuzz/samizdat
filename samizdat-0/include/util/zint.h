/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Safe integer functions.
 */

#ifndef _UTIL_ZINT_H_
#define _UTIL_ZINT_H_

#include <stddef.h>

/**
 * Converts a `zint` to a `zchar`, asserting that the value is in fact in
 * range for same.
 */
zchar zcharFromZint(zint value);

/**
 * Performs `abs(x)` (unary absolute value), detecting overflow. Returns
 * a success flag, and stores the result in the indicated pointer if
 * non-`NULL`.
 *
 * **Note:** The only possible overflow case is `abs(ZINT_MIN)`.
 */
bool zintAbs(zint *result, zint x);

/**
 * Performs `x + y`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintAdd(zint *result, zint x, zint y);

/**
 * Performs `x &&& y`. Returns `true`, and stores the result in the
 * indicated pointer if non-`NULL`. This function never fails; the success
 * flag is so that it can be used equivalently to the other similar functions
 * in this library.
 */
inline bool zintAnd(zint *result, zint x, zint y) {
    if (result != NULL) {
        *result = x & y;
    }

    return true;
}

/**
 * Gets the bit size (highest-order significant bit number, plus one)
 * of the given `zint`, assuming sign-extended representation. For example,
 * this is `1` for both `0` and `-1` (because both can be represented with
 * *just* a single sign bit); and this is `2` for `1` (because it requires
 * one value bit and one sign bit).
 */
zint zintBitSize(zint value);

/**
 * Performs bit extraction `(x >>> y) &&& 1`, detecting errors. Returns a
 * success flag, and stores the result in the indicated pointer if non-`NULL`.
 * For `y >= ZINT_BITS`, this returns the sign bit.
 *
 * **Note:** The only possible errors are when `y < 0`.
 */
bool zintBit(zint *result, zint x, zint y);

/**
 * Performs `x / y` (trucated division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
bool zintDiv(zint *result, zint x, zint y);

/**
 * Performs `x // y` (Euclidean division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
bool zintDivEu(zint *result, zint x, zint y);

/**
 * Performs `x % y` (that is, remainder after truncated division, with the
 * result sign matching `x`), detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This will not fail if an infinite-size int implementation
 * would succeed. In particular, `ZINT_MIN % -1` succeeds and returns `0`.
 */
bool zintMod(zint *result, zint x, zint y);

/**
 * Performs `x %% y` (that is, remainder after Euclidean division, with the
 * result sign always positive), detecting overflow. Returns a success flag,
 * and stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This will not fail if an infinite-size int implementation
 * would succeed. In particular, `ZINT_MIN %% -1` succeeds and returns `0`.
 */
bool zintModEu(zint *result, zint x, zint y);

/**
 * Performs `x * y`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintMul(zint *result, zint x, zint y);

/**
 * Performs `-x` (unary negation), detecting overflow. Returns a success flag,
 * and stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `-ZINT_MIN`.
 */
bool zintNeg(zint *result, zint x);

/**
 * Performs `!!!x` (unary bitwise complement). Returns `true`,
 * and stores the result in the indicated pointer if non-`NULL`. This
 * function never fails; the success flag is so that it can be used
 * equivalently to the other similar functions in this library.
 */
inline bool zintNot(zint *result, zint x) {
    if (result != NULL) {
        *result = ~x;
    }

    return true;
}

/**
 * Performs `x ||| y`. Returns `true`, and stores the result in the
 * indicated pointer if non-`NULL`. This function never fails; the success
 * flag is so that it can be used equivalently to the other similar functions
 * in this library.
 */
inline bool zintOr(zint *result, zint x, zint y) {
    if (result != NULL) {
        *result = x | y;
    }

    return true;
}

/**
 * Performs `sign(x)`. Returns `true`, and stores the result in the
 * indicated pointer if non-`NULL`. This function never fails; the success
 * flag is so that it can be used equivalently to the other similar functions
 * in this library.
 */
inline bool zintSign(zint *result, zint x) {
    if (result != NULL) {
        *result = (x == 0) ? 0 : ((x < 0) ? -1 : 1);
    }

    return true;
}

/**
 * Performs `x <<< y`, detecting overflow (never losing high-order bits).
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** This defines `(x <<< -y) == (x >>> y)`.
 */
bool zintShl(zint *result, zint x, zint y);

/**
 * Performs `x >>> y`, detecting overflow (never losing high-order bits).
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** This defines `(x >>> -y) == (x <<< y)`.
 */
inline bool zintShr(zint *result, zint x, zint y) {
    // We just define this in terms of `zintShl`, but note the limit test,
    // which ensures that we don't try to calculate `-ZINT_MIN` for `y`.
    return zintShl(result, x, (y <= -ZINT_BITS) ? ZINT_BITS : -y);
}

/**
 * Performs `x - y`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintSub(zint *result, zint x, zint y);

/**
 * Performs `x ^^^ y`. Returns `true`, and stores the result in the
 * indicated pointer if non-`NULL`. This function never fails; the success
 * flag is so that it can be used equivalently to the other similar functions
 * in this library.
 */
inline bool zintXor(zint *result, zint x, zint y) {
    if (result != NULL) {
        *result = x ^ y;
    }

    return true;
}

#endif
