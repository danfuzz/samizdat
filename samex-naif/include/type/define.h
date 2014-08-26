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
 * Expands to a comma-separated pair of selector and builtin function,
 * for the indicated method. This is for use in calls to
 * `symbolTableFromArgs`.
 */
#define SEL_METH(cls, name) \
    SEL_NAME(name), \
    makeBuiltin(SEL_MIN_ARGS_##name, SEL_MAX_ARGS_##name, \
        METH_NAME(cls, name), 0, stringFromUtf8(-1, #cls "." #name)) \

/** Variable definition for a method selector. */
#define SEL_DEF(name) \
    zvalue SEL_NAME(name) = NULL

/**
 * Performs initialization of the indicated method selector.
 */
#define SEL_INIT(name) \
    do { \
        SEL_NAME(name) = selectorFromUtf8(-1, #name); \
    } while (0)

#endif
