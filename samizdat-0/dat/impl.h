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
    DAT_VALUE_MAGIC = 0x600f1e57,

    /** Required byte alignment for values. */
    DAT_VALUE_ALIGNMENT = 8
};

/**
 * Common fields across all values. Used as a header for other types.
 */
typedef struct DatValue {
    /** Magic number. */
    uint32_t magic;

    /** Data type. */
    ztype type;

    /** Size. Meaning varies depending on `type`. */
    zint size;
} DatValue;

/**
 * Intlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Signed value as an array of `zint` elements, in little-endian order. */
    zint elems[0];
} DatIntlet;

/**
 * Listlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** List elements, in index order. */
    zvalue elems[0];
} DatListlet;

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    zvalue key;
    zvalue value;
} zmapping;

/**
 * Maplet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** List of mappings, in key-sorted order. */
    zmapping elems[0];
} DatMaplet;

/**
 * Uniqlet info.
 */
typedef struct {
    /** Uniqlet unique id */
    zint id;

    /** Sealer / unsealer key */
    void *key;

    /** Sealed box payload value */
    void *value;
} UniqletInfo;

/**
 * Uniqlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Uniqlet info */
    UniqletInfo info;
} DatUniqlet;

/**
 * Allocates memory, sized to include a `DatValue` header plus the
 * indicated number of extra bytes. The `DatValue` header is
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
zorder datIntletCompare(zvalue v1, zvalue v2);

/**
 * Compares listlets.
 */
zorder datListletCompare(zvalue v1, zvalue v2);

/**
 * Compares maplets.
 */
zorder datMapletCompare(zvalue v1, zvalue v2);

/**
 * Compares uniqlets.
 */
zorder datUniqletCompare(zvalue v1, zvalue v2);

#endif
