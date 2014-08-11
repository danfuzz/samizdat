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

#include "type/Class.h"
#include "type/Function.h"
#if DAT_USE_METHOD_TABLE
#include "type/Selector.h"
#else
#include "type/Generic.h"
#endif
#include "type/String.h"

/** Performs binding of the indicated method. */
#if DAT_USE_METHOD_TABLE
#define METH_BIND(cls, name) \
    do { \
        classAddPrimitiveMethod( \
            CLS_##cls, SEL_NAME(name), \
            SEL_MIN_ARGS_##name, SEL_MAX_ARGS_##name, \
            METH_NAME(cls, name), #cls "." #name); \
    } while (0)
#else
#define METH_BIND(cls, name) \
    do { \
        genericBindPrim(SEL_NAME(name), CLS_##cls, METH_NAME(cls, name), \
            #cls ":" #name); \
    } while (0)
#endif

/** Variable definition for a method selector. */
#define SEL_DEF(name) \
    zvalue SEL_NAME(name) = NULL

/**
 * Performs initialization of the indicated method selector.
 */
#if DAT_USE_METHOD_TABLE
#define SEL_INIT(name) \
    do { \
        SEL_NAME(name) = selectorFromName(stringFromUtf8(-1, #name)); \
    } while (0)
#else
#define SEL_INIT(name) \
    do { \
        SEL_NAME(name) = makeGeneric( \
            SEL_MIN_ARGS_##name, SEL_MAX_ARGS_##name, \
            stringFromUtf8(-1, #name)); \
        datImmortalize(SEL_NAME(name)); \
    } while (0)
#endif

#endif
