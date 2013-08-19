/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Function` data type
 */

#ifndef _TYPE_FUNCTION_H_
#define _TYPE_FUNCTION_H_

#include "pb.h"


/** Type value for in-model type `Function`. */
extern zvalue TYPE_Function;

/**
 * Constructs and returns a function with the given argument
 * restrictions, optional associated closure state, and optional name
 * (used when producing stack traces). `minArgs` must be non-negative,
 * and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue makeFunction(zint minArgs, zint maxArgs, zfunction function,
    zvalue name);

#endif
