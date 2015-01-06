// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Builtin` class
//

#ifndef _TYPE_BUILTIN_H_
#define _TYPE_BUILTIN_H_

#include "type/Value.h"


/** Class value for in-model class `Builtin`. */
extern zvalue CLS_Builtin;

/** Return value from `builtinGetState` (see which). */
typedef struct {
    /** Number of state slots. */
    zint size;

    /** Pointer to the array of state slots. */
    zvalue *arr;
} BuiltinState;

/**
 * Constructs and returns a builtin with the given argument restrictions and
 * optional name (used when producing stack traces). `minArgs` must be
 * non-negative, and `maxArgs` must be either greater than `minArgs` or `-1`
 * to indicate that there is no limit. `stateSize` must be non-negative, and
 * indicates how much space to add for mutable value slots.
 */
zvalue makeBuiltin(zint minArgs, zint maxArgs, zfunction function,
    zint stateSize, zvalue name);

/**
 * Gets the mutable state of the given builtin.
 */
BuiltinState builtinGetState(zvalue builtin);

#endif
