/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Builtin` data type
 */

#ifndef _TYPE_BUILTIN_H_
#define _TYPE_BUILTIN_H_

#include "pb.h"
#include "type/Callable.h"


/** Type value for in-model type `Builtin`. */
extern zvalue TYPE_Builtin;

/**
 * Constructs and returns a builtin with the given argument
 * restrictions, optional associated closure state, and optional name
 * (used when producing stack traces). `minArgs` must be non-negative,
 * and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue makeBuiltin(zint minArgs, zint maxArgs, zfunction function,
    zvalue name);

#endif
