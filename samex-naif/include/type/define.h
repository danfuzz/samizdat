// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Definitions for defining new classes and functions.
//
// This is meant to be `#include`d from non-header source files that define
// classes and/or functions.

#ifndef _TYPE_DEFINE_H_
#define _TYPE_DEFINE_H_

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Function.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "type/String.h"

/**
 * Expands to a comma-separated pair of symbol and builtin function,
 * for the indicated method. This is for use in calls to
 * `symbolTableFromArgs`.
 */
#define SYM_METH(cls, name) \
    SYM_NAME(name), \
    makeBuiltin(SYM_MIN_ARGS_##name, SYM_MAX_ARGS_##name, \
        METH_NAME(cls, name), 0, stringFromUtf8(-1, #cls "." #name)) \

/** Variable definition for a method symbol. */
#define SYM_DEF(name) \
    zvalue SYM_NAME(name) = NULL

/**
 * Performs initialization of the indicated method symbol.
 */
#define SYM_INIT(name) \
    do { \
        SYM_NAME(name) = symbolFromUtf8(-1, #name); \
    } while (0)

/**
 * Implementation declaration for a method on the given class with the
 * given name.
 */
#define METH_IMPL(cls, name) \
    static zvalue METH_NAME(cls, name)( \
        zvalue thisFunction, zint argCount, const zvalue *args)

/**
 * Expands to a comma-separated pair of symbol and builtin function,
 * for the indicated method. This is for use in calls to
 * `symbolTableFromArgs`.
 */
#define METH_BIND(cls, name) \
    symbolFromUtf8(-1, #name), \
    MAKE_##cls##_##name()


//
// Method implementation declarations. Each of these is identical except
// for the argument requirements (not including a `ths` argument). The
// names are `...args` or `...minArgs_maxArgs` with `rest` used to indicate
// an unbounded number of arguments.
//

#define METH_IMPL_MIN_MAX(cls, name, minArgs, maxArgs) \
    static zvalue METH_NAME(cls, name)( \
            zvalue thisFunction, zint argCount, const zvalue *args); \
    static zvalue MAKE_##cls##_##name(void) { \
        return makeBuiltin(minArgs, maxArgs, METH_NAME(cls, name), 0, \
            stringFromUtf8(-1, #cls "." #name)); \
    } \
    static zvalue METH_NAME(cls, name)( \
            zvalue thisFunction, zint argCount, const zvalue *args)

#define METH_IMPL_0(cls, name) \
    static zvalue IMPL_##cls##_##name(zvalue); \
    METH_IMPL_MIN_MAX(cls, name, 1, 1) { \
        return IMPL_##cls##_##name(args[0]); \
    } \
    static zvalue IMPL_##cls##_##name(zvalue ths)

#define METH_IMPL_1(cls, name, a0) \
    static zvalue IMPL_##cls##_##name(zvalue, zvalue); \
    METH_IMPL_MIN_MAX(cls, name, 2, 2) { \
        return IMPL_##cls##_##name(args[0], args[1]); \
    } \
    static zvalue IMPL_##cls##_##name(zvalue ths, zvalue a0)

#define METH_IMPL_2(cls, name, a0, a1) \
    static zvalue IMPL_##cls##_##name(zvalue, zvalue, zvalue); \
    METH_IMPL_MIN_MAX(cls, name, 3, 3) { \
        return IMPL_##cls##_##name(args[0], args[1], args[2]); \
    } \
    static zvalue IMPL_##cls##_##name(zvalue ths, zvalue a0, zvalue a1)

#define METH_IMPL_rest(cls, name, aRest) \
    static zvalue IMPL_##cls##_##name(zvalue, zint, zvalue); \
    METH_IMPL_MIN_MAX(cls, name, 0, -1) { \
        return IMPL_##cls##_##name(args[0], argCount - 1, &args[1]); \
    } \
    static zvalue IMPL_##cls##_##name(zvalue ths, \
            zint aRest##Size, zvalue *aRest)

#define METH_IMPL_0_1(cls, name, a0) \
    static zvalue IMPL_##cls##_##name(zvalue, zvalue); \
    METH_IMPL_MIN_MAX(cls, name, 1, 2) { \
        return IMPL_##cls##_##name(args[0], (argCount > 1) ? args[1] : NULL); \
    } \
    static zvalue IMPL_##cls##_##name(zvalue ths, zvalue a0)

#endif
