// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#ifndef _DAT_CALL_H_
#define _DAT_CALL_H_

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function, and `args` must be a list or `NULL` (the latter treated like
 * an empty list).
 */
zvalue funApply(zvalue function, zvalue args);

/**
 * Calls a function with the given list of arguments. `function` must be
 * a `Function`, and `argCount` must be non-negative. If `argCount` is
 * positive, then `args` must not be `NULL`. In addition all elements of
 * `args` must be non-`NULL`.
 *
 * **Note:** Since in the vast majority of cases it's statically known that
 * `args[*]` is non-`NULL`, those checks are not performed here. If the
 * checks in question need to be performed, then they need to be done on
 * the caller side, e.g. with calls to `datNonVoid()`.
 */
zvalue funCall(zvalue function, zint argCount, const zvalue *args);

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
#define DAT_ARG_COUNT(...) \
    DAT_ARG_COUNT0(__VA_ARGS__, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define DAT_ARG_COUNT0(...) DAT_ARG_COUNT1(__VA_ARGS__)
#define DAT_ARG_COUNT1(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, \
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, n, ...) n
#define FUN_CALL(...) \
    FUN_CALL0(DAT_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
#define FUN_CALL0(argCount, ...) \
    DAT_CONCAT(funCallWith, argCount)(__VA_ARGS__)

/**
 * `METH_APPLY(name, args)`: Calls a method by (unadorned) name,
 * with a variable number of arguments passed as a list.
 */
#define METH_APPLY(name, args) funApply(SYM_NAME(name), args)

/**
 * `METH_CALL(name, arg, ...)`: Calls a method by (unadorned) name,
 * with a variable number of arguments passed in the usual C style.
 */
#define METH_CALL(name, ...) FUN_CALL(SYM_NAME(name), __VA_ARGS__)


//
// Helpers for `FUN_CALL` and `METH_CALL`. Each of these calls a given
// function with a different number of (particular) arguments.
//

inline zvalue funCallWith0(zvalue function) {
    return funCall(function, 0, NULL);
}

inline zvalue funCallWith1(zvalue function, zvalue arg0) {
    return funCall(function, 1, &arg0);
}

inline zvalue funCallWith2(zvalue function, zvalue arg0, zvalue arg1) {
    zvalue args[] = { arg0, arg1 };
    return funCall(function, 2, args);
}

inline zvalue funCallWith3(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2) {
    zvalue args[] = { arg0, arg1, arg2 };
    return funCall(function, 3, args);
}

inline zvalue funCallWith4(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3) {
    zvalue args[] = { arg0, arg1, arg2, arg3 };
    return funCall(function, 4, args);
}

inline zvalue funCallWith5(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4 };
    return funCall(function, 5, args);
}

inline zvalue funCallWith6(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4, arg5 };
    return funCall(function, 6, args);
}

inline zvalue funCallWith7(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6 };
    return funCall(function, 7, args);
}

inline zvalue funCallWith8(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7 };
    return funCall(function, 8, args);
}

inline zvalue funCallWith9(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8) {
    zvalue args[] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
    return funCall(function, 9, args);
}

inline zvalue funCallWith10(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9
    };
    return funCall(function, 10, args);
}

inline zvalue funCallWith11(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10
    };
    return funCall(function, 11, args);
}

inline zvalue funCallWith12(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11
    };
    return funCall(function, 12, args);
}

inline zvalue funCallWith13(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12
    };
    return funCall(function, 13, args);
}

inline zvalue funCallWith14(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13
    };
    return funCall(function, 14, args);
}

inline zvalue funCallWith15(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13, zvalue arg14) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13, arg14
    };
    return funCall(function, 15, args);
}

inline zvalue funCallWith16(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13, arg14, arg15
    };
    return funCall(function, 16, args);
}

inline zvalue funCallWith17(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
        zvalue arg16) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13, arg14, arg15, arg16
    };
    return funCall(function, 17, args);
}

inline zvalue funCallWith18(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
        zvalue arg16, zvalue arg17) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13, arg14, arg15, arg16, arg17
    };
    return funCall(function, 18, args);
}

inline zvalue funCallWith19(zvalue function, zvalue arg0, zvalue arg1,
        zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
        zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
        zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
        zvalue arg16, zvalue arg17, zvalue arg18) {
    zvalue args[] = {
        arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18
    };
    return funCall(function, 19, args);
}

#endif