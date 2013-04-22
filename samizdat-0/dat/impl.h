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

    /** Integer value. See `datIntletFromInt()` about range restriction. */
    int32_t value;
} DatIntlet;

/**
 * Stringlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Characters of the stringlet, in index order. */
    zchar elems[0];
} DatStringlet;

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
    /** Uniqlet unique id. */
    zint id;

    /** Sealer / unsealer key. */
    void *key;

    /** Sealed box payload value. */
    void *value;
} UniqletInfo;

/**
 * Uniqlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Uniqlet info. */
    UniqletInfo info;
} DatUniqlet;

/**
 * Highlet info.
 */
typedef struct {
    /** Type tag. Never `NULL`. */
    zvalue type;

    /** Associated value. Possibly `NULL`. */
    zvalue value;
} HighletInfo;

/**
 * Highlet structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Highlet info. */
    HighletInfo info;
} DatHighlet;

/**
 * Allocates memory, sized to include a `DatValue` header plus the
 * indicated number of extra bytes. The `DatValue` header is
 * initialized with the indicated type and size.
 */
zvalue datAllocValue(ztype type, zint size, zint extraBytes);

/**
 * Returns whether the given value (which must be valid) has an
 * `n`th element, according to its defined size. This is only
 * useful with some types.
 */
bool datHasNth(zvalue value, zint n);

/**
 * Compares intlets.
 */
zorder datIntletOrder(zvalue v1, zvalue v2);

/**
 * Compares stringlets.
 */
zorder datStringletOrder(zvalue v1, zvalue v2);

/**
 * Compares listlets.
 */
zorder datListletOrder(zvalue v1, zvalue v2);

/**
 * Compares maplets.
 */
zorder datMapletOrder(zvalue v1, zvalue v2);

/**
 * Compares uniqlets.
 */
zorder datUniqletOrder(zvalue v1, zvalue v2);

/**
 * Compares highlets.
 */
zorder datHighletOrder(zvalue v1, zvalue v2);

#endif
