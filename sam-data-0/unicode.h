/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Unicode utilities
 */

#ifndef _UNICODE_H_
#define _UNICODE_H_

#include "sam-data.h"

/**
 * Decodes a UTF-8 encoded code point from the given string of the
 * given size in bytes, storing via the given `zint *`. If the decoded
 * value isn't needed, it is valid to pass `result` as `NULL`. Returns
 * a pointer to the position just after the bytes that were decoded.
 */
const zbyte *samUtf8DecodeOne(const zbyte *string, zint stringBytes,
                              zint *result);

/**
 * Gets the decoded size (the number of encoded Unicode code points)
 * of a UTF-8 encoded string of the given size in bytes.
 */
zint samUtf8DecodeStringSize(const zbyte *string, zint stringBytes);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zint`s. The buffer must be sufficiently
 * large to hold the result of decoding.
 */
void samUtf8DecodeStringToInts(const zbyte *string, zint stringBytes,
                               zint *result);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zvalue`s. The buffer must be sufficiently
 * large to hold the result of decoding. Each of the decoded values
 * is an intlet.
 */
void samUtf8DecodeStringToValues(const zbyte *string, zint stringBytes,
                                 zvalue *result);

#endif
