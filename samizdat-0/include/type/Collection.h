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


/** Generic `cat(collection, more*)`: Documented in spec. */
extern zvalue GFN_cat;

/** Generic `del(collection, key)`: Documented in spec. */
extern zvalue GFN_del;

/** Generic `get(collection, key)`: Documented in spec. */
extern zvalue GFN_get;

/** Generic `keyList(collection)`: Documented in spec. */
extern zvalue GFN_keyList;

/** Generic `nth(collection, n)`: Documented in spec. */
extern zvalue GFN_nth;

/** Generic `put(collection, key, value)`: Documented in spec. */
extern zvalue GFN_put;

/** Generic `reverse(sequence)`: Documented in spec. */
extern zvalue GFN_reverse;

/** Generic `size(collection)`: Documented in spec. */
extern zvalue GFN_size;

/** Generic `slice(collection, start, end?)`: Documented in spec. */
extern zvalue GFN_slice;

/**
 * Asserts that the given size accommodates accessing the `n`th element.
 * This includes asserting that `n >= 0`. Note that all non-negative `n`
 * are valid for accessing ints (their size notwithstanding).
 */
void assertNth(zint size, zint n);

/**
 * Like `assertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void assertNthOrSize(zint size, zint n);

/**
 * Validates and converts the `start` and optional `end` arguments to
 * a `slice` call, based on having a collection of the given `size`.
 * On success, stores the start and end values through the given pointers.
 * On failure, terminates the runtime with an error.
 */
void collConvertSliceArgs(zint *startPtr, zint *endPtr, zint size,
        zint argCount, const zvalue *args);

/**
 * Validates the given `key` to use for a `get` style function on a sequence.
 * Returns the int value for a valid `key` (a non-negative `Int`), or
 * `-1` if not.
 */
zint collNthIndexLenient(zvalue key);

/**
 * Returns an index to use for an `nth` style function, given a collection
 * `size` and client-supplied index `n`. This returns `-1` to indicate that
 * the caller should in turn return `NULL`. This is strict in that
 * blatantly-invalid `n`s (non-int or negative int) cause runtime termination.
 */
zint collNthIndexStrict(zint size, zvalue n);

/**
 * Calls the `get` generic.
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
 * Calls the `put` generic.
 */
zvalue collPut(zvalue coll, zvalue key, zvalue value);

/**
 * Returns an index to use for an `put` style function, given a collection
 * `size` and client-supplied index `n`. This returns `-1` to indicate that
 * the caller should in turn return `NULL`. This is strict in that
 * all invalid `n` (non-int, negative int, or `> size`) cause runtime
 * termination.
 */
zint collPutIndexStrict(zint size, zvalue n);

/**
 * Gets the size of the given collection, as a `zint`.
 */
zint collSize(zvalue coll);

/**
 * Binds the standard methods for a `Sequence` type. That is, this
 * is for collections whose keys are a zero-based `Int` sequence.
 * In particular, this binds `get` and `keyList`.
 */
void seqBind(zvalue type);

#endif
