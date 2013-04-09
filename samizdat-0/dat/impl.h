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

#include "dat.h"
#include "util.h"


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

/**
 * Allocates memory, sized to include a `SamValue` header plus the
 * indicated number of extra bytes. The `SamValue` header is
 * initialized with the indicated type and size.
 */
zvalue datAllocValue(ztype type, zint size, zint extraBytes);

/**
 * Allocates a listlet of the given size.
 */
zvalue datAllocListlet(zint size);

/**
 * Gets the array of `zvalue` elements from a listlet.
 */
zvalue *datListletElems(zvalue listlet);

/**
 * Compares intlets.
 */
zcomparison datIntletCompare(zvalue v1, zvalue v2);

/**
 * Compares listlets.
 */
zcomparison datListletCompare(zvalue v1, zvalue v2);

/**
 * Compares maplets.
 */
zcomparison datMapletCompare(zvalue v1, zvalue v2);

/**
 * Compares uniqlets.
 */
zcomparison datUniqletCompare(zvalue v1, zvalue v2);

#endif
