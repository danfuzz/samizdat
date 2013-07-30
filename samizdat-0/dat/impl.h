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
     * TODO: Does GC marking of a value of the given type.
     */
    // void (*gcMark)(zvalue);
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

    /** Size. Meaning varies depending on `type`. */
    zint size;
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

    /** Characters of the string, in index order. */
    zchar elems[/*flexible*/];
} DatString;

/**
 * List structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** List elements, in index order. */
    zvalue elems[/*flexible*/];
} DatList;

/**
 * Map structure.
 */
typedef struct {
    /** Value header. */
    DatHeader header;

    /** List of mappings, in key-sorted order. */
    zmapping elems[/*flexible*/];
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
zvalue datAllocValue(ztype type, zint size, zint extraBytes);

/**
 * Asserts that the given value is a valid `zvalue`, and that its size
 * accommodates accessing the `n`th element. This includes asserting that
 * `n >= 0`. Note that all non-negative `n` are valid for accessing
 * ints (their size notwithstanding).
 */
void datAssertNth(zvalue value, zint n);

/**
 * Like `datAssertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void datAssertNthOrSize(zvalue value, zint n);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * on the given value.
 */
void datAssertSliceRange(zvalue value, zint start, zint end);

/**
 * Compares derived values for equality. Only called when the sizes are
 * the same.
 */
bool datDerivEq(zvalue v1, zvalue v2);

/**
 * Marks derived value contents for garbage collection.
 */
void datDerivMark(zvalue value);

/**
 * Compares derived values for order.
 */
zorder datDerivOrder(zvalue v1, zvalue v2);

/**
 * Returns whether the given value (which must be valid) has an
 * `n`th element, according to its defined size. This is only
 * useful with some types.
 */
bool datHasNth(zvalue value, zint n);

/**
 * Compares ints for equality. Only called when the sizes are the same.
 */
bool datIntEq(zvalue v1, zvalue v2);

/**
 * Compares ints for order.
 */
zorder datIntOrder(zvalue v1, zvalue v2);

/**
 * Compares lists for equality. Only called when the sizes are the same.
 */
bool datListEq(zvalue v1, zvalue v2);

/**
 * Marks list contents for garbage collection.
 */
void datListMark(zvalue value);

/**
 * Compares lists for order.
 */
zorder datListOrder(zvalue v1, zvalue v2);

/**
 * Clears the contents of the map lookup cache.
 */
void datMapClearCache(void);

/**
 * Compares maps for equality. Only called when the sizes are the same.
 */
bool datMapEq(zvalue v1, zvalue v2);

/**
 * Marks map contents for garbage collection.
 */
void datMapMark(zvalue value);

/**
 * Compares maps for order.
 */
zorder datMapOrder(zvalue v1, zvalue v2);

/**
 * Compares strings for equality. Only called when the sizes are the same.
 */
bool datStringEq(zvalue v1, zvalue v2);

/**
 * Compares strings for order.
 */
zorder datStringOrder(zvalue v1, zvalue v2);

/**
 * Frees uniqlet contents during garbage collection.
 */
void datUniqletFree(zvalue value);

/**
 * Marks uniqlet contents for garbage collection.
 */
void datUniqletMark(zvalue value);

/**
 * Compares uniqlets for order.
 */
zorder datUniqletOrder(zvalue v1, zvalue v2);

#endif
