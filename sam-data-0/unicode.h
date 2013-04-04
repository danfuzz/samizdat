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
 * Decodes a UTF-8 encoded code point from the given string, storing
 * via the given `zint *`. Returns a pointer to the position just after
 * the bytes that were decoded.
 */
const zbyte *samDecodeUtf8(const zbyte *string, zint *result);

#endif
