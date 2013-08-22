/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `String` data type
 */

#ifndef _TYPE_STRING_H_
#define _TYPE_STRING_H_

#include "pb.h"
#include "type/Collection.h"


/** Type value for in-model type `String`. */
extern zvalue TYPE_String;

/** The standard value `""`. */
extern zvalue EMPTY_STRING;

/**
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue stringFromUtf8(zint stringBytes, const char *string);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue stringFromZchar(zchar value);

/**
 * Gets the string built from the given array of `zchar`s, of
 * the given size.
 */
zvalue stringFromZchars(zint size, const zchar *chars);

/**
 * Encodes the given string as UTF-8 into the given buffer of the
 * given size in bytes. The buffer must be large enough to hold the entire
 * encoded result plus a terminating `'\0'` byte; if not, this function
 * will complain and exit the runtime. To be clear, the result *is*
 * `'\0'`-terminated.
 *
 * **Note:** If the given string possibly contains any `U+0` code points,
 * then the only "safe" way to use the result is as an explicitly-sized
 * buffer. (For example, `strlen()` might "lie".)
 */
void utf8FromString(zint resultSize, char *result, zvalue string);

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
 * Copies all the characters of the given string into the given result
 * array, which must be sized large enough to hold all of them.
 */
void zcharsFromString(zchar *result, zvalue string);

#endif
