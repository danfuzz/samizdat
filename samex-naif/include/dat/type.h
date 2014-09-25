// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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
 * Prototype for an underlying C function corresponding to an in-model
 * function (value of type `Function`).
 */
typedef zvalue (*zfunction)(
    zvalue thisFunction, zint argCount, const zvalue *args);

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    /** The key. */
    zvalue key;

    /** The value. */
    zvalue value;
} zmapping;

#endif
