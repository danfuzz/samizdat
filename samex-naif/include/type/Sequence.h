// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Sequence` protocol
//
// **Note:** There is no in-model value `Sequence`.
//
// **Note:** Because `sequence` per se is an awkward prefix, instead the
// suggestive prefix `seq` is used.
//

#ifndef _TYPE_SEQUENCE_H_
#define _TYPE_SEQUENCE_H_

#include <stdbool.h>

#include "type/Collection.h"  // Because all sequences are collections.


/** Generic `reverse(sequence)`: Documented in spec. */
extern zvalue GFN_reverse;

/** Generic `sliceExclusive(collection, start, end?)`: Documented in spec. */
extern zvalue GFN_sliceExclusive;

/** Generic `sliceInclusive(collection, start, end?)`: Documented in spec. */
extern zvalue GFN_sliceInclusive;

/**
 * Validates and converts the `start` and optional `end` arguments to
 * a `slice{Ex,In}clusive` call, based on having a collection of the given
 * `size`. On success, stores the start (inclusive) and end (exclusive, always)
 * values through the given pointers. For an empty range, returns `0` for
 * both values. For a void range, returns `-1` for both values. On type
 * failure, terminates the runtime with an error.
 */
void seqConvertSliceArgs(zint *startPtr, zint *endPtr, bool inclusive,
        zint size, zint argCount, const zvalue *args);

/**
 * Validates the given `key` to use for a `get` style function on a sequence.
 * Returns the int value for a valid `key` (a non-negative `Int`), or
 * `-1` if not.
 */
zint seqNthIndexLenient(zvalue key);

/**
 * Returns an index to use for an `nth` style function, given a collection
 * `size` and client-supplied index `n`. This returns `-1` to indicate that
 * the caller should in turn return `NULL`. This is strict in that
 * blatantly-invalid `n`s (non-int) cause runtime termination.
 */
zint seqNthIndexStrict(zint size, zvalue n);

/**
 * Returns an index to use for an `put` style function, given a collection
 * `size` and client-supplied index `n`. This returns `-1` to indicate that
 * the caller should in turn return `NULL`. This is strict in that
 * all invalid `n` (non-int, negative int, or `> size`) cause runtime
 * termination.
 */
zint seqPutIndexStrict(zint size, zvalue n);

/**
 * Binds the standard methods for a `Sequence` type. That is, this
 * is for collections whose keys form a zero-based `Int` sequence.
 * In particular, this binds `collect`, `get`, `keyList`, `nextValue`,
 * `nthMapping`, and `valueList`.
 */
void seqBind(zvalue type);

#endif
