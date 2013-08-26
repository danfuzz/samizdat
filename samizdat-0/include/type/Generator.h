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
 *
 * **Note:** Because `generator` per se is an awkward prefix, instead the
 * suggestive prefix `gtr` is used.
 */

#ifndef _TYPE_GENERATOR_H_
#define _TYPE_GENERATOR_H_

#include "pb.h"


/** Generic `collect()`: Documenetd in spec. */
extern zvalue GFN_collect;

/** Generic `next(box)`: Documenetd in spec. */
extern zvalue GFN_next;


#endif
