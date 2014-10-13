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
#include "type/Record.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"

//
// Function implementation declarations. Each of these is identical except
// for the argument requirements. The names are of the form `...args` or
// `...minArgs_maxArgs` with `rest` used to indicate an unbounded number of
// arguments.
//

/**
 * Calls the constructor for the indicated function, returning an in-model
 * function value.
 */
#define FUNC_VALUE(name) (MAKE_##name())

/**
 * Generalized function implemenation declaration, used by the
 * argument-specific ones.
 */
#define FUNC_IMPL_MIN_MAX(name, minArgs, maxArgs) \
    static zvalue name(zvalue, zint, const zvalue *); \
    static zvalue MAKE_##name(void) { \
        return makeBuiltin(minArgs, maxArgs, name, 0, \
            symbolFromUtf8(-1, #name)); \
    } \
    static zvalue name(zvalue _function, zint _argsSize, const zvalue *_args)

#define FUNC_IMPL_0(name) \
    static zvalue IMPL_##name(void); \
    FUNC_IMPL_MIN_MAX(name, 0, 0) { \
        return IMPL_##name(); \
    } \
    static zvalue IMPL_##name(void)

#define FUNC_IMPL_1(name, a0) \
    static zvalue IMPL_##name(zvalue); \
    FUNC_IMPL_MIN_MAX(name, 1, 1) { \
        return IMPL_##name(_args[0]); \
    } \
    static zvalue IMPL_##name(zvalue a0)

#define FUNC_IMPL_2(name, a0, a1) \
    static zvalue IMPL_##name(zvalue, zvalue); \
    FUNC_IMPL_MIN_MAX(name, 2, 2) { \
        return IMPL_##name(_args[0], _args[1]); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1)

#define FUNC_IMPL_3(name, a0, a1, a2) \
    static zvalue IMPL_##name(zvalue, zvalue, zvalue); \
    FUNC_IMPL_MIN_MAX(name, 3, 3) { \
        return IMPL_##name(_args[0], _args[1], _args[2]); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1, zvalue a2)

#define FUNC_IMPL_rest(name, aRest) \
    static zvalue IMPL_##name(zint, const zvalue *); \
    FUNC_IMPL_MIN_MAX(name, 0, -1) { \
        return IMPL_##name(_argsSize, _args); \
    } \
    static zvalue IMPL_##name(zint aRest##Size, const zvalue *aRest)

#define FUNC_IMPL_1_2(name, a0, a1) \
    static zvalue IMPL_##name(zvalue, zvalue); \
    FUNC_IMPL_MIN_MAX(name, 1, 2) { \
        return IMPL_##name(_args[0], (_argsSize > 1) ? _args[1] : NULL); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1)

#define FUNC_IMPL_1_rest(name, a0, aRest) \
    static zvalue IMPL_##name(zvalue, zint, const zvalue *); \
    FUNC_IMPL_MIN_MAX(name, 1, -1) { \
        return IMPL_##name(_args[0], _argsSize - 1, &_args[1]); \
    } \
    static zvalue IMPL_##name(zvalue a0, zint aRest##Size, const zvalue *aRest)

#define FUNC_IMPL_2_3(name, a0, a1, a2) \
    static zvalue IMPL_##name(zvalue, zvalue, zvalue); \
    FUNC_IMPL_MIN_MAX(name, 2, 3) { \
        return IMPL_##name( \
            _args[0], \
            _args[1], \
            (_argsSize > 2) ? _args[2] : NULL); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1, zvalue a2)

#define FUNC_IMPL_2_4(name, a0, a1, a2, a3) \
    static zvalue IMPL_##name(zvalue, zvalue, zvalue, zvalue); \
    FUNC_IMPL_MIN_MAX(name, 2, 4) { \
        return IMPL_##name( \
            _args[0], \
            _args[1], \
            (_argsSize > 2) ? _args[2] : NULL, \
            (_argsSize > 3) ? _args[3] : NULL); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1, zvalue a2, zvalue a3)

#define FUNC_IMPL_2_rest(name, a0, a1, aRest) \
    static zvalue IMPL_##name(zvalue, zvalue, zint, const zvalue *); \
    FUNC_IMPL_MIN_MAX(name, 2, -1) { \
        return IMPL_##name(_args[0], _args[1], _argsSize - 2, &_args[2]); \
    } \
    static zvalue IMPL_##name(zvalue a0, zvalue a1, \
            zint aRest##Size, const zvalue *aRest)


//
// Method implementation declarations and associated binder. Each of the
// `METH_IMPL*` macros expands to a `FUNC_IMPL*` macro with one extra
// argument `ths`. Similarly, each of the `CMETH_IMPL*` results takes
// an extra argument `thsClass`.
//

// For both class and instance methods.

/**
 * Macro to create and return a method table. Arguments are expected to be
 * calls to `METH_BIND` or `CMETH_BIND`.
 */
#define METH_TABLE(...) (cm_new_SymbolTable(__VA_ARGS__, NULL))

// Instance method implementation macros.

/**
 * Expands to a comma-separated pair of symbol and builtin function,
 * for the indicated method.
 */
#define METH_BIND(cls, name) \
    SYM(name), \
    FUNC_VALUE(cls##_##name)

#define METH_IMPL_0(cls, name)         FUNC_IMPL_1(cls##_##name, ths)
#define METH_IMPL_1(cls, name, a0)     FUNC_IMPL_2(cls##_##name, ths, a0)
#define METH_IMPL_2(cls, name, a0, a1) FUNC_IMPL_3(cls##_##name, ths, a0, a1)
#define METH_IMPL_rest(cls, name, aRest) \
    FUNC_IMPL_1_rest(cls##_##name, ths, aRest)
#define METH_IMPL_0_1(cls, name, a0)   FUNC_IMPL_1_2(cls##_##name, ths, a0)
#define METH_IMPL_1_2(cls, name, a0, a1) \
    FUNC_IMPL_2_3(cls##_##name, ths, a0, a1)

// Class method implementation macros. Structure is identical to the instance
// method macros, above.

#define CMETH_BIND(cls, name) \
    SYM(name), \
    FUNC_VALUE(class_##cls##_##name)

#define CMETH_IMPL_1(cls, name, a0) \
    FUNC_IMPL_2(class_##cls##_##name, thsClass, a0)
#define CMETH_IMPL_rest(cls, name, aRest) \
    FUNC_IMPL_1_rest(class_##cls##_##name, ths, aRest)
#define CMETH_IMPL_0_1(cls, name, a0) \
    FUNC_IMPL_1_2(class_##cls##_##name, thsClass, a0)
#define CMETH_IMPL_1_2(cls, name, a0, a1) \
    FUNC_IMPL_2_3(class_##cls##_##name, thsClass, a0, a1)
#define CMETH_IMPL_1_rest(cls, name, a0, aRest) \
    FUNC_IMPL_2_rest(class_##cls##_##name, ths, a0, aRest)


//
// Symbols
//

/** Variable definition for a symbol. */
#define SYM_DEF(name) \
    zvalue SYM(name) = NULL

/**
 * Performs initialization of the indicated symbol, with the given string
 * name.
 */
#define SYM_INIT_WITH(name, value) \
    do { \
        SYM(name) = symbolFromUtf8(-1, value); \
    } while (0)

/**
 * Performs initialization of the indicated symbol, with a name identical
 * to the given variable name.
 */
#define SYM_INIT(name) SYM_INIT_WITH(name, #name)

#endif
