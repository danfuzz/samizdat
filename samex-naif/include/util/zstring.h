// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// zstring data type
//

#ifndef _UTIL_ZSTRING_H_
#define _UTIL_ZSTRING_H_

#include "ztype.h"

/**
 * Struct to hold a sized Unicode string. **Note:** This has a pointer to the
 * characters, not the characters themselves.
 */
typedef struct {
    /** Number of characters in the string. */
    zint size;

    /** The characters. */
    const zchar *chars;
} zstring;

/**
 * Copies all the characters of the given `zstring` into the given result
 * array, which must be sized large enough to hold all of them.
 */
void arrayFromZstring(zchar *result, zstring string);

/**
 * Like `utf8FromZstring`, except this returns an allocated buffer containing
 * the result.
 */
char *utf8DupFromZstring(zstring string);

/**
 * Encodes the given `zstring` as UTF-8 into the given buffer of the
 * given size in bytes, returning the number of bytes written to. The buffer
 * must be large enough to hold the entire encoded result plus a terminating
 * `'\0'` byte; if not, this function will complain and exit the runtime.
 * To be clear, the result *is* `'\0'`-terminated, and the `'\0'` is included
 * in the result count.
 *
 * **Note:** If the `string` possibly contains any `U+0` code points,
 * then the only "safe" way to use the result is as an explicitly-sized
 * buffer. (For example, `strlen()` might "lie.")
 */
zint utf8FromZstring(zint resultSize, char *result, const zstring string);

/**
 * Gets the number of bytes required to encode the given `zstring`
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromZstring(const zstring string);

/**
 * Compares two `zstring`s for equality.
 */
bool zstringEq(zstring string1, zstring string2);

/**
 * Compares two `zstring`s for order.
 */
zorder zstringOrder(zstring string1, zstring string2);

#endif
