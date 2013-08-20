/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Collection` protocols
 *
 * **Note:** There is no in-model value `Collection`. There are a few
 * different subsets of collection-like functionality which may eventually
 * be represented as protocols or similar.
 *
 * **Note:** Because `collection` per se is an awkward prefix, instead the
 * suggestive prefix `coll` is used.
 */

#ifndef _TYPE_COLLECTION_H_
#define _TYPE_COLLECTION_H_

#include "pb.h"


/** Generic `get(collection, key)`: Documented in spec. */
extern zvalue GFN_get;

/** Generic `nth(collection, n)`: Documented in spec. */
extern zvalue GFN_nth;

/** Generic `size(collection)`: Documented in spec. */
extern zvalue GFN_size;

/**
 * Gets the size of the given collection, as a `zint`.
 */
zint collSize(zvalue coll);

/**
 * Binds the standard methods for a `Sequence` type. That is, this
 * is for collections whose keys are a zero-based `Int` sequence.
 */
void seqBind(zvalue type);

#endif
