// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Definitions for declaring new classes and functions.
//
// This is `#include`d from `Value.h` and so is pretty much always available
// in in-model headers.

#ifndef _TYPE_DECLARE_H_
#define _TYPE_DECLARE_H_

#include "dat.h"

/**
 * C source name for a `zfunction` implementation with the given name.
 * The result is a prefixed version of the given name.
 */
#define FUN_IMPL_NAME(name) FUN_IMPL_##name

/**
 * Declaration for a `zfunction` implementation with the given name. Can be
 * used as either a prototype or a top-of-implementation declaration.
 */
#define FUN_IMPL_DECL(name) \
    zvalue FUN_IMPL_NAME(name)( \
        zvalue thisFunction, zint argCount, const zvalue *args)

/** C function name for a method on the given class with the given name. */
#define METH_NAME(cls, name) cls##_##name

/** Declaration for a method on the given class with the given name. */
#define METH_IMPL(cls, name) \
    static zvalue METH_NAME(cls, name)( \
        zvalue thisFunction, zint argCount, const zvalue *args)

#endif
