/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Lowest-level utilities
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include "ztype.h"

#include <stdbool.h>
#include <stddef.h>


/*
 * Message Declarations
 */

enum {
    /** Magic value used to identify stack giblets. */
    UTIL_GIBLET_MAGIC = 0x57ac61b1
};

/**
 * Function added to the "death context", which is expected to return
 * some sort of interesting string.
 */
typedef char *(*zcontextFunction)(void *state);

/**
 * Stack "giblet". This is a struct placed on the (C) stack, for use
 * when producing a stack trace upon process death.
 */
typedef struct UtilStackGiblet {
    /** Identifying magic value. */
    zint magic;

    /** Giblet one layer up on the stack. */
    struct UtilStackGiblet *pop;

    /** Function to call to produce a line of context. */
    zcontextFunction function;

    /** State to pass to `function`. */
    void *state;
} UtilStackGiblet;

/** The current top-of-stack of giblets. */
extern UtilStackGiblet *utilStackTop;

/**
 * Defines a giblet for the current function. Use this at the point a
 * stack trace for the call would be valid.
 */
#define UTIL_TRACE_START(function, state) \
    UtilStackGiblet stackGiblet = { \
        UTIL_GIBLET_MAGIC, utilStackTop, (function), (state) \
    }; \
    do { \
        utilStackTop = &stackGiblet; \
    } while(0)

/**
 * Ends the scope for a giblet. Use this as close to function return as
 * possible.
 */
#define UTIL_TRACE_END() \
    do { \
        utilStackTop = stackGiblet.pop; \
    } while(0)

/**
 * Emits a debugging message. Arguments are as with `printf()`.
 */
void note(const char *format, ...)
    __attribute__((format (printf, 1, 2)));

/**
 * Dies (aborts the process) with the given message. Arguments are as
 * with `printf()`. If there is any active stack context (more
 * `deathPush*()`es than `deathPop()`s), then that context is appended
 * to the death report.
 */
void die(const char *format, ...)
    __attribute__((noreturn))
    __attribute__((format (printf, 1, 2)));


/*
 * Allocation Declarations
 */

/**
 * Allocates zeroed-out memory of the indicated size (in bytes).
 */
void *utilAlloc(zint size);

/**
 * Frees memory previously allocated by `utilAlloc`.
 */
void utilFree(void *memory);

/**
 * Returns whether this appears to be a pointer to heap-allocated memory
 * (though not necessarily the start of an allocation).
 */
bool utilIsHeapAllocated(void *memory);


/*
 * UTF-8 Declarations
 */

/**
 * Gets the decoded size (the number of encoded Unicode code points)
 * of a UTF-8 encoded string of the given size in bytes.
 */
zint utf8DecodeStringSize(zint stringBytes, const char *string);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zchar`s. The buffer must be sufficiently
 * large to hold the result of decoding.
 */
void utf8DecodeCharsFromString(zchar *result,
        zint stringBytes, const char *string);

/**
 * Encodes a single Unicode code point as UTF-8, writing it to the
 * given string, which must be large enough to hold it. Returns a
 * pointer to the position just after what was encoded. If `string` is
 * passed as `NULL`, this doesn't encode but still returns the
 * would-be encoded size in pointer form (i.e. `(char *) NULL +
 * size`).
 */
char *utf8EncodeOne(char *result, zint ch);


/*
 * `zint` Declarations
 */

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
 * **Note:** This only succeeds in cases that `x / y` succeeds, that is,
 * `ZINT_MIN % -1` fails.
 */
bool zintMod(zint *result, zint x, zint y);

/**
 * Performs `x %% y` (that is, remainder after Euclidean division, with the
 * result sign always positive), detecting overflow. Returns a success flag,
 * and stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This only succeeds in cases that `x // y` succeeds, that is,
 * `ZINT_MIN %% -1` fails.
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
