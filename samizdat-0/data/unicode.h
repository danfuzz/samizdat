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

#include "data.h"

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zvalue`s. The buffer must be sufficiently
 * large to hold the result of decoding. Each of the decoded values
 * is an intlet.
 */
void samUtf8DecodeStringToValues(const char *string, zint stringBytes,
                                 zvalue *result);

#endif
