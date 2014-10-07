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
// In cases where the only win here is to remove boilerplate, the definition
// is a macro which expands to a straightforward method call. In cases where
// a method can take different numbers of arguments, the macro is a varargs
// one; in these cases, the C compiler will not catch calls with an improper
// number of arguments.
//
// Several of the definitions here are functions which take or return C types
// instead of just `zvalue`, and in some cases the contract is *slightly*
// different, in order to be more convenient to the C callers. All such
// variance is documented.

#ifndef _METHOD_CALLS_H_
#define _METHOD_CALLS_H_

#include "dat.h"

/**
 * Calls `x.cat(...)`. **Note:** This is a macro.
 */
#define cm_cat(...) (METH_CALL(cat, __VA_ARGS__))

/**
 * Calls `x.debugString()`, converting the result to a `char *`. The
 * caller is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *cm_debugString(zvalue x);

/**
 * Calls `x.fetch()`. **Note:** This is a macro.
 */
#define cm_fetch(x) (METH_CALL(fetch, (x)))

/**
 * Calls `x.get(key)`. **Note:** This is a macro.
 */
#define cm_get(x, key) (METH_CALL(get, (x), (key)))

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
 * Calls `x.put(key, value)`. **Note:** This is a macro.
 */
#define cm_put(x, key, value) (METH_CALL(put, (x), (key), (value)))

/**
 * Calls `x.store(...)`. **Note:** This is a macro.
 */
#define cm_store(...) (METH_CALL(store, __VA_ARGS__))

/**
 * Calls `x.toString()`. **Note:** This is a macro.
 */
#define cm_toString(x) (METH_CALL(toString, (x)))

/**
 * Calls `x.get_data()`. **Note:** This is a macro.
 */
#define get_data(x) (METH_CALL(get_data, (x)))

/**
 * Calls `x.get_name()`. **Note:** This is a macro.
 */
#define get_name(x) (METH_CALL(get_name, (x)))

/**
 * Calls `x.get_size()`, converting the result to a `zint`.
 */
zint get_size(zvalue x);

#endif
