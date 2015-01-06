// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Safe integer functions
//
// Notes
// -----
//
// With regard to detecting overflow, C compilers are allowed to optimize
// overflowing operations in surprising ways. Therefore, the usual
// recommendation for portable code is to detect overflow by inspection of
// operands, not results. The choices for what to detect here are informed by,
// but not exactly the same as, the CERT "AIR" recommendations.
//
// With regard to division and modulo, this file implements both
// traditional truncating division and Euclidean division, each with its
// corresponding modulo operation. These are implemented to assume the C99
// definition of division and remainder, in particular that division is
// truncating and that the sign of a modulo result is the same as the sign
// of the dividend (the left-hand argument).
//
// This file is implemented *almost* entirely without assuming an underlying
// int representation, not for its own sake but instead because that's what's
// most readable (and is also not horrendously inefficient). In a couple
// cases, though, the code does assume that ints are represented as
// twos-complement binary values, since that makes the code easier to
// understand.
//
// References:
//
// * [As-If Infinitely Ranged Integer Model, Second
//   Edition](http://www.cert.org/archive/pdf/10tn008.pdf)
//
// * [CERT C Secure Coding
//   Standard](https://www.securecoding.cert.org/confluence/display/
//   seccode/CERT+C+Secure+Coding+Standard)
//
// * [Division and Modulus for Computer
//   Scientists](http://legacy.cs.uu.nl/daan/download/papers/divmodnote.pdf)
//
// * [The Euclidean Definition of the Functions div and
//   mod](https://biblio.ugent.be/publication/314490/file/452146.pdf)
//
// * [Modulo operation
//   (Wikipedia)](http://en.wikipedia.org/wiki/Modulo_operation)
//
// * [Safe IOP Library](https://code.google.com/p/safe-iop/)
//

#ifndef _UTIL_ZINT_H_
#define _UTIL_ZINT_H_

#include <stddef.h>


//
// Private definitions
//

/**
 * Common check for `zintDiv*` functions.
 */
inline bool zintCanDiv(zint x, zint y) {
    if (y == 0) {
        // Divide by zero.
        return false;
    }

    if ((x == ZINT_MIN) && (y == -1)) {
        // Overflow: `-ZINT_MIN` is not representable as a `zint`.
        return false;
    }

    return true;
}


//
// Public definitions
//

/**
 * Converts a `zint` to a `zchar`, detecting overflow. Returns
 * a success flag, and stores the result in the indicated pointer if
 * non-`NULL`.
 */
inline bool zcharFromZint(zchar *result, zint value) {
    if ((value < 0) || (value > ZCHAR_MAX)) {
        return false;
    }

    if (result != NULL) {
        *result = (zchar) value;
    }

    return true;
}

/**
 * Performs `abs(x)` (unary absolute value), detecting overflow. Returns
 * a success flag, and stores the result in the indicated pointer if
 * non-`NULL`.
 *
 * **Note:** The only possible overflow case is `abs(ZINT_MIN)`.
 */
inline bool zintAbs(zint *result, zint x) {
    if (x == ZINT_MIN) {
        return false;
    }

    if (result != NULL) {
        *result = (x < 0) ? -x : x;
    }

    return true;
}

/**
 * Performs `x + y`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
inline bool zintAdd(zint *result, zint x, zint y) {
    // If the signs are opposite or either argument is zero, then overflow
    // is impossible. The two clauses here are for the same-sign-and-not-zero
    // cases. Each one is of the form: Given a {positive, negative} `y`,
    // what is the {largest, smallest} `x` that won't overflow?
    if (((y > 0) && (x > (ZINT_MAX - y))) ||
        ((y < 0) && (x < (ZINT_MIN - y)))) {
        return false;
    }

    if (result != NULL) {
        *result = x + y;
    }

    return true;
}

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
 * Performs bit extraction `(x >>> y) &&& 1`, detecting errors. Returns a
 * success flag, and stores the result in the indicated pointer if non-`NULL`.
 * For `y >= ZINT_BITS`, this returns the sign bit.
 *
 * **Note:** The only possible errors are when `y < 0`.
 */
inline bool zintBit(zint *result, zint x, zint y) {
    if (y < 0) {
        return false;
    }

    if (result != NULL) {
        if (y >= ZINT_BITS) {
            y = ZINT_BITS - 1;
        }

        *result = (x >> y) & 1;
    }

    return true;
}

/**
 * Gets the bit size (highest-order significant bit number, plus one)
 * of the given `zint`, assuming sign-extended representation.
 *
 * For example, the bit size is `1` for both `0` and `-1` (because both can
 * be represented with *just* a single sign bit); and this is `2` for `1`
 * (because it requires one value bit and one sign bit).
 */
inline zint zintBitSize(zint value) {
    if (value < 0) {
        value = ~value;
    }

    // "Binary-search" style implementation. Many compilers have a
    // built-in "count leading zeroes" function, but we're aiming
    // for portability here.

    zint result = 1;  // +1 in that we want size, not zero-based bit number.
    uint64_t uv = (uint64_t) value;  // Use `uint` to account for `-ZINT_MAX`.

    if (uv >= ((uint64_t) 1 << 32)) { result += 32; uv >>= 32; }
    if (uv >= ((uint64_t) 1 << 16)) { result += 16; uv >>= 16; }
    if (uv >= ((uint64_t) 1 << 8))  { result +=  8; uv >>=  8; }
    if (uv >= ((uint64_t) 1 << 4))  { result +=  4; uv >>=  4; }
    if (uv >= ((uint64_t) 1 << 2))  { result +=  2; uv >>=  2; }
    if (uv >= ((uint64_t) 1 << 1))  { result +=  1; uv >>=  1; }
    return result + uv;
}

/**
 * Version of `zintBitSize` that uses a result-pointer form factor.
 * Returns `true`, and stores the result in the indicated pointer if
 * non-`NULL`. This function never fails; the success flag is so that it
 * can be used equivalently to the other similar functions in this library.
 */
inline bool zintSafeBitSize(zint *result, zint value) {
    if (result != NULL) {
        *result = zintBitSize(value);
    }

    return true;
}

/**
 * Performs `x / y` (trucated division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
inline bool zintDiv(zint *result, zint x, zint y) {
    if (!zintCanDiv(x, y)) {
        return false;
    }

    if (result != NULL) {
        *result = x / y;
    }

    return true;
}

/**
 * Performs `x // y` (Euclidean division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
inline bool zintDivEu(zint *result, zint x, zint y) {
    if (!zintCanDiv(x, y)) {
        return false;
    }

    if (result != NULL) {
        zint quo = x / y;
        zint rem = x % y;
        if (rem < 0) {
            if (y > 0) { quo--; }
            else       { quo++; }
        }
        *result = quo;
    }

    return true;
}

/**
 * Performs `x % y` (that is, remainder after truncated division, with the
 * result sign matching `x`), detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This will not fail if an infinite-size int implementation
 * would succeed. In particular, `ZINT_MIN % -1` succeeds and returns `0`.
 */
inline bool zintMod(zint *result, zint x, zint y) {
    if (y == 0) {
        // Divide by zero.
        return false;
    }

    if (result != NULL) {
        *result = x % y;
    }

    return true;
}

/**
 * Performs `x %% y` (that is, remainder after Euclidean division, with the
 * result sign always positive), detecting overflow. Returns a success flag,
 * and stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This will not fail if an infinite-size int implementation
 * would succeed. In particular, `ZINT_MIN %% -1` succeeds and returns `0`.
 */
inline bool zintModEu(zint *result, zint x, zint y) {
    if (y == 0) {
        // Divide by zero.
        return false;
    }

    if (result != NULL) {
        zint rem = x % y;
        if (rem < 0) {
            if (y > 0) { rem += y; }
            else       { rem -= y; }
        }
        *result = rem;
    }

    return true;
}

/**
 * Performs `x * y`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
inline bool zintMul(zint *result, zint x, zint y) {
    // This is broken down by sign of the arguments, with zeros getting
    // an easy pass-through. The outer test is for the sign of `x`. Each
    // inner test checks the sign of `y` with a subsequent required
    // range check, similar to how the `add` and `sub` implementations are
    // written.

    if (x > 0) {
        if (((y > 0) && (x > (ZINT_MAX / y))) ||
            ((y < 0) && (y < (ZINT_MIN / x)))) {
            return false;
        }
    } else if (x < 0) {
        if (((y > 0) && (x < (ZINT_MIN / y))) ||
            ((y < 0) && (y < (ZINT_MAX / x)))) {
            return false;
        }
    }

    if (result != NULL) {
        *result = x * y;
    }

    return true;
}

/**
 * Performs `-x` (unary negation), detecting overflow. Returns a success flag,
 * and stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `-ZINT_MIN`.
 */
inline bool zintNeg(zint *result, zint x) {
    if (x == ZINT_MIN) {
        return false;
    }

    if (result != NULL) {
        *result = -x;
    }

    return true;
}

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
inline bool zintShl(zint *result, zint x, zint y) {
    zint res;

    if ((x == 0) || (y == 0)) {
        res = x;
    } else if (y > 0) {
        // Left shift (and `x` is non-zero). There's definite loss of bits if
        // `y` is more than the size of an int. With a potentially in-range
        // `y`, we have to check based on the sign of `x`, ensuring that a
        // positive `x` is small enough or a negative `x` is large enough
        // that the shift couldn't lose its most significant non-sign bit.
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
inline bool zintSub(zint *result, zint x, zint y) {
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
