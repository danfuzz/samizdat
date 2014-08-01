// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Generic` class
//

#ifndef _TYPE_GENERIC_H_
#define _TYPE_GENERIC_H_

#include "type/Function.h"

/** Class value for in-model class `Generic`. */
extern zvalue CLS_Generic;

/**
 * Adds a class-to-function binding to the given generic. `generic` must be
 * a generic function, `cls` must be a class, and `function` must be a valid
 * `Function`. The class must not have already been bound in the given generic,
 * and the generic must not be sealed.
 */
void genericBind(zvalue generic, zvalue cls, zvalue function);

/**
 * Adds a class-to-C-function binding to the given generic. `generic` must
 * be a generic function, `cls` must be a class, and `function` must be a
 * valid `zfunction`. The class must not have already been bound in the given
 * generic, and the generic must not be sealed. An optional `builtinName`
 * becomes the name of the bound builtin.
 */
void genericBindPrim(zvalue generic, zvalue cls, zfunction function,
    const char *builtinName);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void genericSeal(zvalue generic);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and name (used for mapping to a selector and when producing
 * stack traces). It is initially unsealed and without any bindings. `minArgs`
 * must be at least `1`, and `maxArgs` must be either greater than `minArgs`
 * or `-1` to indicate that there is no limit.
 */
zvalue makeGeneric(zint minArgs, zint maxArgs, zvalue name);

#endif
