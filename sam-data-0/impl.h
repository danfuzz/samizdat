/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Private structure implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "sam-data.h"

enum {
    /** "Magic number" for value validation. */
    SAM_VALUE_MAGIC = 0x600f1e57,

    /** Required byte alignment for values. */
    SAM_VALUE_ALIGNMENT = 8
};

/**
 * Common fields across all values. Used as a header for other types.
 */
typedef struct SamValue {
    /** Magic number. */
    uint32_t magic;

    /** Data type. */
    ztype type;

    /** Size. Meaning varies depending on `type`. */
    zint size;
} SamValue;

/**
 * Intlet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    /** Signed value as an array of `zint` elements, in little-endian order. */
    zint elems[0];
} SamIntlet;

/**
 * Listlet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    /** List elements, in index order. */
    zvalue elems[0];
} SamListlet;

/**
 * Maplet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    /** List of mappings, in key-sorted order. */
    zmapping elems[0];
} SamMaplet;

/**
 * Uniqlet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    /** Uniqlet unique id */
    zint id;
} SamUniqlet;

#endif
