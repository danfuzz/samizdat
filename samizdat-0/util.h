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


/*
 * Message functions
 */

/**
 * Function added to the "death context", which is expected to return
 * some sort of interesting string.
 */
typedef char *(*zcontextFunction)(void *state);

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

/**
 * Adds a stack layer to the current context, for emission in case
 * of death.
 */
void debugPush(zcontextFunction function, void *state);

/**
 * Pops the top layer of death / debug context.
 */
void debugPop(void);

/**
 * Gets a "mark" representing the current debug stack depth.
 */
zint debugMark(void);

/**
 * Resets the debug stack to what it was at the given mark.
 */
void debugReset(zint mark);


/*
 * Allocation functions
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
 * String functions
 */

/**
 * Gets a pointer just past the end of the given string, asserting
 * validity of same. This just validates this as an address range,
 * not as valid string contents.
 */
const char *utilStringEnd(zint stringBytes, const char *string);


/*
 * Unicode functions
 */

/**
 * Asserts that the given `zint` value is valid as a Unicode
 * code point.
 */
void uniAssertValid(zint value);

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

#endif
