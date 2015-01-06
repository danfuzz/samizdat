// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

//
// Functions
//

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
    zvalue FUN_IMPL_NAME(name)(zvalue thisFunction, zarray args)


//
// Strings
//

/** Variable name for a string. */
#define STRING_NAME(name) STRING_##name

/** Declaration for a string. */
#define STRING_DECL(name) \
    extern zvalue STRING_NAME(name) \
    // No semicolon here, so that use sites require it.


//
// Symbols
//

/** Variable name for a symbol. */
#define SYM(name) SYM_##name

/** Variable name for a symbol index. */
#define SYMIDX(name) SYMIDX_##name

/** Declaration for a symbol. */
#define SYM_DECL(name) \
    extern zint SYMIDX(name); \
    extern zvalue SYM(name) \
    // No semicolon here, so that use sites require it.

#endif
