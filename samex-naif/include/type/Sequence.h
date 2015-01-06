// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

#include "type/Value.h"


/** Standard implementation for `Sequence.get`. */
extern zvalue FUN_Sequence_get;

/** Standard implementation for `Sequence.keyList`. */
extern zvalue FUN_Sequence_keyList;

/** Standard implementation for `Sequence.reverseNth`. */
extern zvalue FUN_Sequence_reverseNth;

/** Standard implementation for `Sequence.sliceGeneral`. */
extern zvalue FUN_Sequence_sliceGeneral;

/**
 * Validates and converts the `start` and optional `end` arguments to
 * a `slice{Ex,In}clusive` call, based on having a collection of the given
 * `size`. On success, stores the start (inclusive) and end (exclusive, always)
 * values through the given pointers. For an empty range, returns `0` for
 * both values. For a void range, returns `-1` for both values. On type
 * failure, terminates the runtime with an error.
 */
void seqConvertSliceArgs(zint *startPtr, zint *endPtr, bool inclusive,
        zint size, zvalue startArg, zvalue endArg);

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

#endif
