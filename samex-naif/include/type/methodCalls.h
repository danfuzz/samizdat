// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
 * Calls `x.castFrom(value)`. **Note:** This is a macro.
 */
#define cm_castFrom(x, value) (METH_CALL((x), castFrom, (value)))

/**
 * Calls `x.cat(...)`. **Note:** This is a macro, and due to limitations on
 * C macros, this must be passed at least one argument.
 */
#define cm_cat(x, ...) (METH_CALL((x), cat, __VA_ARGS__))

/**
 * Calls `x.debugString()`, converting the result to a `char *`. The
 * caller is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *cm_debugString(zvalue x);

/**
 * Calls `x.fetch()`. **Note:** This is a macro.
 */
#define cm_fetch(x) (METH_CALL((x), fetch))

/**
 * Calls `x.get(key)`. **Note:** This is a macro.
 */
#define cm_get(x, key) (METH_CALL((x), get, (key)))

/**
 * Calls `ClassName.new(...)`. `ClassName` is fixed to be a proper C name
 * for a class. **Note:** This is a macro.
 */
#define cm_new(clsName, ...) (METH_CALL(CLS_##clsName, new, __VA_ARGS__))

/**
 * Calls `ClassName.new()` or `ClassName.new(value)`. `ClassName` is fixed to
 * be a proper C name for a class, and `value` is allowed to be `NULL`, in
 * which case the constructor is called with no arguments. This is meant to
 * make it easy to call box or box-like constructors, hence the name.
 * **Note:** This is a macro.
 */
#define cm_newBox(clsName, value) (cm_newBox0(CLS_##clsName, (value)))
zvalue cm_newBox0(zvalue cls, zvalue value);

/**
 * Does the equivalent of calling `List.new(...)`, except that this uses
 * lower-layer functionality, making this function safe to use before the
 * class `List` is fully initialized. **Note:** This is a macro, and due to
 * limitations on C macros, this must be passed at least one argument.
 */
#define cm_new_List(...) (listFromZarray( \
    (zarray) {CALL_ARG_COUNT(__VA_ARGS__), CALL_ARG_ARRAY(__VA_ARGS__)}))

/**
 * Does the equivalent of calling `Record.new(name, SymbolTable.new(...))`.
 * **Note:** This is a macro, and due to limitations on C macros, this must be
 * passed at least one key-value argument pair.
 */
#define cm_new_Record(name, ...) (recFromZarray( \
    (name), \
    (zarray) {CALL_ARG_COUNT(__VA_ARGS__), CALL_ARG_ARRAY(__VA_ARGS__)}))

/**
 * Does the equivalent of calling `SymbolTable.new(...)`, except that this
 * uses lower-layer functionality, making this function safe to use before the
 * class `SymbolTable` is fully initialized. **Note:** This is a macro, and
 * due to limitations on C macros, this must be passed at least one argument.
 */
#define cm_new_SymbolTable(...) (symtabFromZarray( \
    (zarray) {CALL_ARG_COUNT(__VA_ARGS__), CALL_ARG_ARRAY(__VA_ARGS__)}))

/**
 * Calls `x.nth(index)`, converting the given `zint` index to an `Int`.
 */
zvalue cm_nth(zvalue x, zint index);

/**
 * Calls `x.cmpOrder(other)`, except that the return value is of type
 * `zorder`, and this reports a fatal error if given incomparable values.
 *
 * **Note:** The constants `{ ZLESS, ZSAME, ZMORE }` can be used when looking
 * at results.
 */
zorder cm_order(zvalue x, zvalue other);

/**
 * Calls `x.store()` or `x.store(value)`. This function always takes an
 * argument, which is allowed to be `NULL`. If `NULL`, this calls the `store`
 * method with no arguments. If non-`NULL`, it calls the method with one
 * argument.
 */
zvalue cm_store(zvalue x, zvalue value);

/**
 * Calls `x.get_data()`. **Note:** This is a macro.
 */
#define get_data(x) (METH_CALL((x), get_data))

/**
 * Calls `x.get_name()`. **Note:** This is a macro.
 */
#define get_name(x) (METH_CALL((x), get_name))

/**
 * Calls `x.get_size()`, converting the result to a `zint`.
 */
zint get_size(zvalue x);

#endif
