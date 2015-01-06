// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `String` class
//

#ifndef _TYPE_STRING_H_
#define _TYPE_STRING_H_

#include "type/Core.h"
#include "type/Sequence.h"
#include "util.h"


/** Class value for in-model class `String`. */
extern zvalue CLS_String;

/** The standard value `""`. */
extern zvalue EMPTY_STRING;

/**
 * Compares two strings for equality. This is the same as calling
 * `cmpEq()` on the strings, except that it terminates with an error if
 * either argument is *not* a string, and it doesn't require a fully running
 * system. In particular, it avoids method dispatch.
 */
bool stringEq(zvalue string1, zvalue string2);

/**
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `utfBytes`
 * is passed as `-1`, this relies on `utf` being `\0`-terminated.
 */
zvalue stringFromUtf8(zint utfBytes, const char *utf);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue stringFromZchar(zchar value);

/**
 * Gets the string built from the given `zstring`.
 */
zvalue stringFromZstring(zstring string);

/**
 * Compares two strings for equality. This is the same as calling
 * `cm_order()` on the strings, except that it terminates with an error if
 * either argument is *not* a string, and it doesn't require a fully
 * running system. In particular, it avoids method dispatch.
 */
zorder stringZorder(zvalue string1, zvalue string2);

/**
 * Like `utf8FromString`, except this returns an allocated buffer containing
 * the result.
 */
char *utf8DupFromString(zvalue string);

/**
 * Encodes the given string as UTF-8 into the given buffer of the
 * given size in bytes, returning the number of bytes written to. The buffer
 * must be large enough to hold the entire encoded result plus a terminating
 * `'\0'` byte; if not, this function will complain and exit the runtime.
 * To be clear, the result *is* `'\0'`-terminated, and the `'\0'` is included
 * in the result count.
 *
 * **Note:** If the given string possibly contains any `U+0` code points,
 * then the only "safe" way to use the result is as an explicitly-sized
 * buffer. (For example, `strlen()` might "lie.")
 */
zint utf8FromString(zint resultSize, char *result, zvalue string);

/**
 * Gets the number of bytes required to encode the given string
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromString(zvalue string);

/**
 * Returns the single character of the given string, which must in fact
 * be a single-character string.
 */
zchar zcharFromString(zvalue string);

/**
 * Gets a `zstring` of the given string. The result `chars` shares storage
 * with the `string`. As such, it is important to *not* modify the contents.
 */
zstring zstringFromString(zvalue string);

#endif
