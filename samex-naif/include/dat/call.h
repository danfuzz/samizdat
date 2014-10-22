// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#ifndef _DAT_CALL_H_
#define _DAT_CALL_H_

/**
 * Calls the given `function` with the given list of arguments. `function`
 * must be a function, and `args` must be a list or `NULL` (the latter treated
 * like an empty list).
 */
zvalue funApply(zvalue function, zvalue args);

/**
 * Calls the given `function` with the given list of arguments. `function`
 * must be a function, and `argCount` must be non-negative. If `argCount` is
 * positive, then `args` must not be `NULL`. In addition, all elements of
 * `args` must be non-`NULL`.
 *
 * **Note:** Since in the vast majority of cases it's statically known that
 * `args[*]` is non-`NULL`, those checks are not performed here. If the
 * checks in question need to be performed, then they need to be done on
 * the caller side, e.g. with calls to `datNonVoid()`.
 */
zvalue funCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Calls the method `name` on target `target`, with the given list of
 * `args`. `name` must be a symbol, and `args` must be a list or `NULL` (the
 * latter treated as the empty list).
 */
zvalue methApply(zvalue target, zvalue name, zvalue args);

/**
 * Calls the method `name` on target `target`, with the given list of
 * `args`. `name` must be a symbol, and `argCount` must be non-negative. If
 * `argCount` is positive, then `args` must not be `NULL`. In addition, all
 * elements of `args` must be non-`NULL`.
 *
 * **Note:** Since in the vast majority of cases it's statically known that
 * `args[*]` is non-`NULL`, those checks are not performed here. If the
 * checks in question need to be performed, then they need to be done on
 * the caller side, e.g. with calls to `datNonVoid()`.
 */
zvalue methCall(zvalue target, zvalue name, zint argCount,
        const zvalue *args);

/**
 * Like `methCall()`, except that arguments to the method are passed as
 * individual C arguments, followed by a `NULL` arguement at the end.
 */
zvalue vaMethCall(zvalue target, zvalue name, ...);

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
// Function calling macros
//

/**
 * `FUN_CALL(function, arg, ...)`: Calls a function, with a variable number
 * of arguments passed in the usual C style.
 *
 * This uses some crazy preprocessor stuff to make it all work out. See
 * <http://stackoverflow.com/questions/2632300> and
 * <http://stackoverflow.com/questions/1872220>. Note that the definitions
 * here are slightly different than the answer at those links, to account for
 * the fact that the first argument to the call here is the function, and not
 * a numbered argument.
 */
#define DAT_CONCAT(x, y) x##y
#define DAT_FIRST(x, ...) x
#define DAT_REST(x, ...) __VA_ARGS__
#define DAT_ARG_COUNT(...) \
    DAT_ARG_COUNT0(__VA_ARGS__, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define DAT_ARG_COUNT0(...) DAT_ARG_COUNT1(__VA_ARGS__)
#define DAT_ARG_COUNT1(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, \
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, n, ...) n
#define FUN_CALL(...) \
    funCall(DAT_FIRST(__VA_ARGS__), \
        DAT_ARG_COUNT(__VA_ARGS__), (zvalue[]) { DAT_REST(__VA_ARGS__) })

/**
 * `METH_APPLY(target, name, args)`: Calls a method by (unadorned) name,
 * with a variable number of arguments passed as a list.
 */
#define METH_APPLY(target, name, args) methApply((target), SYM(name), (args))

/**
 * `METH_CALL(target, name, arg, ...)`: Calls a method on a given `target`
 * by (unadorned) name, with a variable number of arguments passed in the
 * usual C style.
 *
 * See <https://stackoverflow.com/questions/26497854/> for info about how
 * this works.
 */
#define METH_ARG_ARRAY(...) ((zvalue[]) { __VA_ARGS__ })
#define METH_ARG_COUNT(...) \
    (sizeof (METH_ARG_ARRAY(__VA_ARGS__)) / sizeof (zvalue))
#define METH_CALL(target, name, ...) \
    methCall((target), SYM(name), \
        METH_ARG_COUNT(__VA_ARGS__), \
        METH_ARG_ARRAY(__VA_ARGS__))

/**
 * `VA_METH_CALL(target, name, arg, ...)`: Calls a method on a given `target`
 * by (unadorned) name, with a variable number of arguments passed in the
 * usual C style. Under the covers, this calls `vaMethCall()` to "parse" the
 * call, and as such it is less efficient than `METH_CALL()`, but unlike that
 * macro, this one can handle any number of arguments.
 */
#define VA_METH_CALL(target, name, ...) \
    vaMethCall((target), SYM(name), __VA_ARGS__, NULL)

#endif
