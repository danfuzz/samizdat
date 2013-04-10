/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Lowest-level utilities
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include "ztype.h"


/*
 * Message functions
 */

/**
 * Emits a debugging message. Arguments are as with `printf()`.
 */
void note(const char *format, ...)
    __attribute__((format (printf, 1, 2)));

/**
 * Dies (aborts the process) with the given message. Arguments are as
 * with `printf()`.
 */
void die(const char *format, ...)
    __attribute__((noreturn))
    __attribute__((format (printf, 1, 2)));


/*
 * Allocation functions
 */

/**
 * Allocates zeroed-out memory of the indicated size (in bytes).
 */
void *zalloc(zint size);


/*
 * String functions
 */

/**
 * Gets a pointer just past the end of the given string, asserting
 * validity of same. This just validates this as an address range,
 * not as valid string contents.
 */
const char *strGetEnd(const char *string, zint stringBytes);


/*
 * Unicode functions
 */

/**
 * Asserts that the given `zint` value is valid as a Unicode
 * code point.
 */
void uniAssertValid(zint value);

/**
 * Decodes a UTF-8 encoded code point from the given string of the
 * given size in bytes, storing via the given `zint *`. If the decoded
 * value isn't needed, it is valid to pass `result` as `NULL`. Returns
 * a pointer to the position just after the bytes that were decoded.
 */
const char *utf8DecodeOne(const char *string, zint stringBytes,
                          zint *result);

/**
 * Gets the decoded size (the number of encoded Unicode code points)
 * of a UTF-8 encoded string of the given size in bytes.
 */
zint utf8DecodeStringSize(const char *string, zint stringBytes);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zint`s. The buffer must be sufficiently
 * large to hold the result of decoding.
 */
void utf8DecodeStringToInts(const char *string, zint stringBytes,
                            zint *result);

/**
 * Encodes a single Unicode code point as UTF-8, writing it to the
 * given string, which must be large enough to hold it. Returns a
 * pointer to the position just after what was encoded. If `string` is
 * passed as `NULL`, this doesn't encode but still returns the
 * would-be encoded size in pointer form (i.e. `(char *) NULL +
 * size`).
 */
char *utf8EncodeOne(char *string, zint ch);

#endif
