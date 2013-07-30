/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
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
 * Core type info.
 */
typedef struct DatType {
    /** Low-layer data type identifier. */
    ztypeId id;

    /** Simple string name for the type. */
    const char *name;

    /**
     * Gets the "size" of a value of the given type, for the appropriate
     * per-type meaning of size.
     */
    zint (*sizeOf)(zvalue);

    /**
     * Does GC marking of a value of the given type.
     */
    void (*gcMark)(zvalue);

    /**
     * Frees a garbage value. Optional (may be `NULL`).
     */
    void (*gcFree)(zvalue);

    /**
     * Compares for equality with another value of the same type. Only
     * ever called when the two values are not `==`.
     */
    bool (*eq)(zvalue, zvalue);

    /**
     * Compares for order with another value of the same type. Only ever
     * called when the two values are not `==`.
     */
    zorder (*order)(zvalue, zvalue);
} DatType;

/**
 * Links and flags used for allocation lifecycle management. Every value is
 * linked into a circularly linked list, which identifies its current
 * fate / classification.
 */
typedef struct GcLinks {
    /** Circular link. */
    struct GcLinks *next;

    /** Circular link. */
    struct GcLinks *prev;

    /** Magic number. */
    uint32_t magic;

    /** Mark bit (used during GC). */
    bool marked;
} GcLinks;

/**
 * Common fields across all values. Used as a header for other types.
 */
typedef struct DatHeader {
    /** Gc links (see above). */
    GcLinks links;

    /** Data type. */
    ztype type;
} DatHeader;

/**
 * Int structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Int value. See `datIntFromZint()` about range restriction. */
    int32_t value;
} DatInt;

/**
 * String structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Number of characters. */
    zint size;

    /** Characters of the string, in index order. */
    zchar elems[/*size*/];
} DatString;

/**
 * List structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Number of elements. */
    zint size;

    /** List elements, in index order. */
    zvalue elems[/*size*/];
} DatList;

/**
 * Map structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Number of mappings. */
    zint size;

    /** List of mappings, in key-sorted order. */
    zmapping elems[/*size*/];
} DatMap;

/**
 * Uniqlet info.
 */
typedef struct {
    /** Uniqlet unique id. */
    zint id;

    /** Dispatch table. */
    DatUniqletDispatch *dispatch;

    /** Sealed box payload value. */
    void *state;
} UniqletInfo;

/**
 * Uniqlet structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Uniqlet info. */
    UniqletInfo info;
} DatUniqlet;

/**
 * Derived value info.
 */
typedef struct {
    /** Type tag. Never `NULL`. */
    zvalue type;

    /** Associated payload data. Possibly `NULL`. */
    zvalue data;
} DerivInfo;

/**
 * Derived value structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** Derived value info. */
    DerivInfo info;
} DatDeriv;

/**
 * Allocates memory, sized to include a `DatHeader` header plus the
 * indicated number of extra bytes. The `DatHeader` header is
 * initialized with the indicated type and size. The resulting value
 * is added to the live reference stack.
 */
zvalue datAllocValue(ztype type, zint extraBytes);

/**
 * Asserts that the given size accommodates accessing the `n`th element.
 * This includes asserting that `n >= 0`. Note that all non-negative `n`
 * are valid for accessing ints (their size notwithstanding).
 */
void datAssertNth(zint size, zint n);

/**
 * Like `datAssertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void datAssertNthOrSize(zint size, zint n);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * for a value of the given size.
 */
void datAssertSliceRange(zint size, zint start, zint end);

/**
 * Clears the contents of the map lookup cache.
 */
void datMapClearCache(void);

#endif
