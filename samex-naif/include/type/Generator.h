/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Generator` protocol
 *
 * **Note:** There is no in-model value `Generator`. That said, there is
 * effectively a `Generator` interface or protocol, in that things that
 * implement the `next` and `collect` generics are generators.
 */

#ifndef _TYPE_GENERATOR_H_
#define _TYPE_GENERATOR_H_

#include "dat.h"


/** Generic `collect(generator)`: Documenetd in spec. */
extern zvalue GFN_collect;

/** Generic `nextValue(generator, box)`: Documenetd in spec. */
extern zvalue GFN_nextValue;

/**
 * "Standard" implementation of `collect`, in terms of `nextValue`.
 * Documented in spec.
 */
extern zvalue FUN_Generator_stdCollect;


#endif
