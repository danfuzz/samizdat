// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Utility functions and macros to call methods from C. This helps remove
// boilerplate from the call sites of commonly-called methods.
//
// Things defined here have two naming conventions:
//
// * `get_<name>(x)` is a function to call the method `x.get_<name>()`.
// * `cm_<name>(x, ...)` is a function to call the method `x.<name>(...)`.
//
// Note that several of these functions take or return C types instead of
// just `zvalue`, and in some cases the contract is *slightly* different, in
// order to be more convenient to the C callers. All such variance is
// documented, though not all variance warrants the third convention (above).
//
// Also note that a couple of these are macros which take varargs. In these
// cases, the C compiler will not catch calls with an improper number of
// arguments.

#ifndef _METHOD_CALLS_H_
#define _METHOD_CALLS_H_

#include "dat.h"

/**
 * Calls `x.cat(...)`. **Note:** This is a macro.
 */
#define cm_cat(...) METH_CALL(cat, __VA_ARGS__)

/**
 * Calls `x.debugString()`, converting the result to a `char *`. The
 * caller is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *cm_debugString(zvalue x);

/**
 * Calls `x.fetch()`.
 */
zvalue cm_fetch(zvalue x);

/**
 * Calls `x.get(key)`.
 */
zvalue cm_get(zvalue x, zvalue key);

/**
 * Calls `x.nth(index)`, converting the given `zint` index to an `Int`.
 */
zvalue cm_nth(zvalue x, zint index);

/**
 * Calls `x.valOrder(other)`, except that the return value is of type
 * `zorder`, and this reports a fatal error if given incomparable values.
 *
 * **Note:** The constants `{ ZLESS, ZSAME, ZMORE }` can be used when looking
 * at results.
 */
zorder cm_order(zvalue x, zvalue other);

/**
 * Calls `x.put(key, value)`.
 */
zvalue cm_put(zvalue x, zvalue key, zvalue value);

/**
 * Calls `x.store()`, with either no arguments or one argument, if `value`
 * is `NULL` or not (respectively). `box` is *not* allowed to be `NULL`.
 */
zvalue cm_store(zvalue x, zvalue value);

/**
 * Calls `x.toString()`.
 */
zvalue cm_toString(zvalue x);

/**
 * Calls `x.get_data()`.
 */
zvalue get_data(zvalue x);

/**
 * Calls `x.get_name()`.
 */
zvalue get_name(zvalue x);

/**
 * Calls `x.get_size()`, converting the result to a `zint`.
 */
zint get_size(zvalue x);

#endif
