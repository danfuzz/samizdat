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
 * Performs `v1 + v2`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintAdd(zint *result, zint v1, zint v2);

/**
 * Gets the bit size (highest-order significant bit number, plus one)
 * of the given `zint`, assuming sign-extended representation. For example,
 * this is `1` for both `0` and `-1` (because both can be represented with
 * *just* a single sign bit).
 */
zint zintBitSize(zint value);

/**
 * Performs `v1 / v2` (trucated division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
bool zintDiv(zint *result, zint v1, zint v2);

/**
 * Performs `v1 // v2` (Euclidean division), detecting overflow and errors.
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** The only possible overflow case is `ZINT_MIN / -1`, and the
 * only other error is division by zero.
 */
bool zintDivEu(zint *result, zint v1, zint v2);

/**
 * Performs bit extraction `(v1 >> v2) & 1`, detecting errors. Returns a
 * success flag, and stores the result in the indicated pointer if non-`NULL`.
 * For `v2 >= ZINT_BITS`, this returns the sign bit.
 *
 * **Note:** The only possible errors are when `v2 < 0`.
 */
bool zintGetBit(zint *result, zint v1, zint v2);

/**
 * Performs `v1 % v2` (that is, remainder after truncated division, with the
 * result sign matching `v1`), detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This only succeeds in cases that `v1 / v2` succeeds, that is,
 * `ZINT_MIN % -1` fails.
 */
bool zintMod(zint *result, zint v1, zint v2);

/**
 * Performs `v1 %% v2` (that is, remainder after Euclidean division, with the
 * result sign matching `v2`), detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 *
 * **Note:** This only succeeds in cases that `v1 / v2` succeeds, that is,
 * `ZINT_MIN %% -1` fails.
 */
bool zintModEu(zint *result, zint v1, zint v2);

/**
 * Performs `v1 * v2`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintMul(zint *result, zint v1, zint v2);

/**
 * Performs `v1 << v2`, detecting overflow (never losing high-order bits).
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** This defines `(v1 << -v2) == (v1 >> v2)`.
 */
bool zintShl(zint *result, zint v1, zint v2);

/**
 * Performs `v1 >> v2`, detecting overflow (never losing high-order bits).
 * Returns a success flag, and stores the result in the indicated pointer
 * if non-`NULL`.
 *
 * **Note:** This defines `(v1 >> -v2) == (v1 << v2)`.
 */
bool zintShr(zint *result, zint v1, zint v2);

/**
 * Performs `v1 - v2`, detecting overflow. Returns a success flag, and
 * stores the result in the indicated pointer if non-`NULL`.
 */
bool zintSub(zint *result, zint v1, zint v2);


#endif
