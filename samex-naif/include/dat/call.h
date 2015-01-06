// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#ifndef _DAT_CALL_H_
#define _DAT_CALL_H_

/**
 * Calls the method `name` on target `target`, with the given list of
 * `args`. `name` must be a symbol, and `args` must be a list or `NULL` (the
 * latter treated as the empty list).
 */
zvalue methApply(zvalue target, zvalue name, zvalue args);

/**
 * Calls the method `name` on target `target`, with the given list of
 * `args`. `name` must be a symbol, and all elements of `args` must be
 * non-`NULL`.
 *
 * **Note:** Since in the vast majority of cases it's statically known that
 * `args[*]` is non-`NULL`, those checks are not performed here. If the
 * checks in question need to be performed, then they need to be done on
 * the caller side, e.g. with calls to `datNonVoid()`.
 */
zvalue methCall(zvalue target, zvalue name, zarray args);

/**
 * Function which should never get called. This is used to wrap calls which
 * aren't allowed to return. Should they return, this function gets called
 * and promptly dies with a fatal error.
 *
 * **Note:** This function is typed to return a `zvalue` (and not void),
 * so that it can be used in contexts that require a return value. This is
 * a quirk of the `noreturn` extension to C, which this function uses.
 */
zvalue mustNotYield(zvalue value)
    __attribute__((noreturn));


//
// Function / method calling macros
//
// See <https://stackoverflow.com/questions/26497854/> for info about how
// the varargs macros work. There's some additional history at
// <http://stackoverflow.com/questions/2632300> and
// <http://stackoverflow.com/questions/1872220>.
//

/**
 * Helper for call macros: Construct an inline `zvalue[]` for all given
 * arguments.
 */
#define CALL_ARG_ARRAY(...) ((zvalue[]) { __VA_ARGS__ })

/**
 * Helper for call macros: Get the number of arguments.
 */
#define CALL_ARG_COUNT(...) \
    (sizeof (CALL_ARG_ARRAY(__VA_ARGS__)) / sizeof (zvalue))

/**
 * `FUN_CALL(function, arg, ...)`: Calls a function, with a variable number
 * of arguments passed in the usual C style.
 */
#define FUN_CALL(func, ...) \
    methCall(func, SYM(call), \
        (zarray) {CALL_ARG_COUNT(__VA_ARGS__), CALL_ARG_ARRAY(__VA_ARGS__)})

/**
 * `METH_APPLY(target, name, args)`: Calls a method by (unadorned) name,
 * with a variable number of arguments passed as a list.
 */
#define METH_APPLY(target, name, args) methApply((target), SYM(name), (args))

/**
 * `METH_CALL_SYM(target, name, arg, ...)`: Calls a method on a given `target`
 * given a symbol name, with a variable number of arguments passed in the
 * usual C style.
 */
#define METH_CALL_SYM(target, name, ...) \
    methCall((target), name, \
        (zarray) {CALL_ARG_COUNT(__VA_ARGS__), CALL_ARG_ARRAY(__VA_ARGS__)})

/**
 * `METH_CALL(target, name, arg, ...)`: Calls a method on a given `target`
 * by (unadorned) name, with a variable number of arguments passed in the
 * usual C style.
 */
#define METH_CALL(target, name, ...) \
    METH_CALL_SYM(target, SYM(name), __VA_ARGS__)

#endif
