// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Utility functions to call methods from C. This helps remove boilerplate
// from commonly-called methods.
//

#ifndef _METHOD_CALLS_H_
#define _METHOD_CALLS_H_

#include "dat.h"

/**
 * Calls `value.get_name()`.
 */
zvalue get_name(zvalue value);

/**
 * Calls `value.debugString()`, converting the result to a `char *`. The
 * caller is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *valDebugString(zvalue value);

/**
 * Calls `value.toString()`.
 */
zvalue valToString(zvalue value);

/**
 * Calls `value.valOrder(other)`, except that the return value is of type
 * `zorder`, and this reports a fatal error if given incomparable values.
 *
 * **Note:** The constants `{ ZLESS, ZSAME, ZMORE }` can be used when looking
 * at results.
 */
zorder valZorder(zvalue value, zvalue other);

#endif
