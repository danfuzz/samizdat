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
#include "type/Generic.h"

#include <stdbool.h>


/** Generic `get(collection, key)`: Documented in spec. */
extern zvalue GFN_get;

/** Generic `nth(collection, n)`: Documented in spec. */
extern zvalue GFN_nth;

/** Generic `size(collection)`: Documented in spec. */
extern zvalue GFN_size;

/**
 * Validates the given `key` to use for a `get` style function on a sequence.
 * Returns `true` to indicate that the `key` is valid (a non-negative `Int`),
 * or `false` if not.
 */
bool collNthIndexLenient(zvalue key);

/**
 * Returns an index to use for an `nth` style function, given a collection
 * `size` and client-supplied index `n`. This returns `-1` to indicate that
 * the caller should in turn return `NULL`. This is strict in that
 * blatantly-invalid `n`s (non-int or negative int) cause runtime termination.
 */
zint collNthIndexStrict(zint size, zvalue n);

/**
 * Calls `get`.
 */
zvalue collGet(zvalue coll, zvalue key);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value.
 */
zvalue collNth(zvalue coll, zint index);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value, and
 * converting a non-void return value &mdash; which must be a single-character
 * `String` &mdash; to a `zint` in the range of a `zchar`. A void return
 * value gets converted to `-1`.
 */
zint collNthChar(zvalue coll, zint index);

/**
 * Gets the size of the given collection, as a `zint`.
 */
zint collSize(zvalue coll);

/**
 * Binds the standard methods for a `Sequence` type. That is, this
 * is for collections whose keys are a zero-based `Int` sequence.
 * In particular, this binds `get` to call through to `nth` when passed
 * a potentially-valid key.
 */
void seqBind(zvalue type);

#endif
