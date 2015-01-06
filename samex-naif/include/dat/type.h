// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#ifndef _DAT_TYPE_H_
#define _DAT_TYPE_H_

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct DatHeader *zvalue;

/** Type for local value stack pointers. */
typedef zvalue *zstackPointer;

/**
 * Low-level sized-array of `zvalue`s.
 */
typedef struct {
    /** Number of elements. */
    zint size;

    /** Pointer to the elements. */
    const zvalue *elems;
} zarray;

/** The empty `zarray`. */
#define EMPTY_ZARRAY ((zarray) {0, NULL})

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    /** The key. */
    zvalue key;

    /** The value. */
    zvalue value;
} zmapping;

/**
 * Low-level sized-array of `zmapping`s.
 */
typedef struct {
    /** Number of elements. */
    zint size;

    /** Pointer to the elements. */
    const zmapping *elems;
} zassoc;

/**
 * Prototype for an underlying C function corresponding to an in-model
 * function.
 */
typedef zvalue (*zfunction)(zvalue thisFunction, zarray args);

#endif
