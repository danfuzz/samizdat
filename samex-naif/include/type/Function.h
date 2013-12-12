/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Function` protocol
 */

#ifndef _TYPE_FUNCTION_H_
#define _TYPE_FUNCTION_H_

#include "pb.h"

#include <stddef.h>


/**
 * Prototype for an underlying C function corresponding to an in-model
 * function (value of type `Function`).
 */
typedef zvalue (*zfunction)(zint argCount, const zvalue *args);

/**
 * Generic `call(function, args*)`: Generic for dispatching to a function
 * calling mechanism (how meta). Documented in spec.
 */
extern zvalue GFN_call;

/** Generic `canCall(function, value)`: Documented in spec. */
extern zvalue GFN_canCall;

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue funApply(zvalue function, zvalue args);

/**
 * Calls a function with the given list of arguments. `function` must be
 * a `Function`, and `argCount` must be non-negative. If `argCount` is
 * positive, then `args` must not be `NULL`.
 */
zvalue funCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Helper for `FUN_CALL`: Calls a function with no arguments.
 */
inline zvalue funCallWith0(zvalue function) {
    return funCall(function, 0, NULL);
}

/**
 * Helper for `FUN_CALL`: Calls a function with one argument.
 */
inline zvalue funCallWith1(zvalue function, zvalue arg0) {
    return funCall(function, 1, &arg0);
}

/**
 * Helper for `FUN_CALL`: Calls a function with two arguments.
 */
inline zvalue funCallWith2(zvalue function, zvalue arg0, zvalue arg1) {
    zvalue args[] = { arg0, arg1 };
    return funCall(function, 2, args);
}

/**
 * Helper for `FUN_CALL`: Calls a function with three arguments.
 */
inline zvalue funCallWith3(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2) {
    zvalue args[] = { arg0, arg1, arg2 };
    return funCall(function, 3, args);
}

/**
 * Helper for `FUN_CALL`: Calls a function with four arguments.
 */
inline zvalue funCallWith4(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3) {
    zvalue args[] = { arg0, arg1, arg2, arg3 };
    return funCall(function, 4, args);
}

/**
 * Helper for `FUN_CALL`: Calls a function with five arguments.
 */
inline zvalue funCallWith5(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4 };
    return funCall(function, 5, args);
}

/**
 * Helper for `FUN_CALL`: Calls a function with six arguments.
 */
inline zvalue funCallWith6(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4, arg5 };
    return funCall(function, 6, args);
}

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
#define PB_CONCAT(x, y) x##y
#define PB_ARG_COUNT(...) \
    PB_ARG_COUNT0(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define PB_ARG_COUNT0(...) PB_ARG_COUNT1(__VA_ARGS__)
#define PB_ARG_COUNT1(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, n, ...) n
#define FUN_CALL(...) \
    FUN_CALL0(PB_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#define FUN_CALL0(argCount, ...) \
    PB_CONCAT(funCallWith, argCount)(__VA_ARGS__)

/**
 * `GFN_CALL(name, arg, ...)`: Calls a generic function by (unadorned) name,
 * with a variable number of arguments passed in the usual C style.
 */
#define GFN_CALL(name, ...) FUN_CALL(GFN_##name, __VA_ARGS__)

#endif
