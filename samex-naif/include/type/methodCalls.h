// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Utility functions to call methods from C. This helps remove boilerplate
// from commonly-called methods.
//
// Functions defined here have three naming conventions:
//
// * `get_<name>(value)` is a function to call the method `value.get_<name>()`.
//
// * `cm_<name>(value, ...)` is a function to call the method
//   `value.<name>(...)`.
//
// Note that several of these functions take or return C types instead of
// just `zvalue`, and in some cases the contract is *slightly* different, in
// order to be more convenient to the C callers. All such variance is
// documented, though not all variance warrants the third convention (above).

#ifndef _METHOD_CALLS_H_
#define _METHOD_CALLS_H_

#include "dat.h"

/**
 * Calls the method `box.store()`, with either no arguments or one argument,
 * if `value` is `NULL` or not (respectively). `box` is *not* allowed to be
 * `NULL`.
 */
zvalue cm_store(zvalue box, zvalue value);

/**
 * Calls `value.get_data()`.
 */
zvalue get_data(zvalue value);

/**
 * Calls `value.get(key)`.
 */
zvalue get(zvalue value, zvalue key);

/**
 * Calls `value.get_name()`.
 */
zvalue get_name(zvalue value);

/**
 * Calls `value.get_size()`, converting the result to a `zint`.
 */
zint get_size(zvalue value);

/**
 * Calls `value.nth(index)`, converting the given `zint` index to an `Int`
 * value.
 */
zvalue nth(zvalue value, zint index);

/**
 * Calls `value.debugString()`, converting the result to a `char *`. The
 * caller is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *cm_debugString(zvalue value);

/**
 * Calls `value.toString()`.
 */
zvalue cm_toString(zvalue value);

/**
 * Calls `value.valOrder(other)`, except that the return value is of type
 * `zorder`, and this reports a fatal error if given incomparable values.
 *
 * **Note:** The constants `{ ZLESS, ZSAME, ZMORE }` can be used when looking
 * at results.
 */
zorder cm_order(zvalue value, zvalue other);

#endif
