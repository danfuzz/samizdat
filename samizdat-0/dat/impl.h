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
 * Links and flags used for allocation lifecycle management. Every value is
 * linked into a circularly linked list, which identifies its current
 * fate / classification.
 */
typedef struct GcLinks {
    struct GcLinks *next;
    struct GcLinks *prev;
    bool marked;
} GcLinks;


/**
 * Common fields across all values. Used as a header for other types.
 */
typedef struct DatValue {
    /** Gc links (see above). */
    GcLinks links;

    /** Magic number. */
    uint32_t magic;

    /** Data type. */
    ztype type;

    /** Size. Meaning varies depending on `type`. */
    zint size;
} DatValue;

/**
 * Int structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Int value. See `datIntFromZint()` about range restriction. */
    int32_t value;
} DatInt;

/**
 * String structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Characters of the string, in index order. */
    zchar elems[0];
} DatString;

/**
 * List structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** List elements, in index order. */
    zvalue elems[0];
} DatList;

/**
 * Map structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** List of mappings, in key-sorted order. */
    zmapping elems[0];
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
    DatValue header;

    /** Uniqlet info. */
    UniqletInfo info;
} DatUniqlet;

/**
 * Token info.
 */
typedef struct {
    /** Type tag. Never `NULL`. */
    zvalue type;

    /** Associated value. Possibly `NULL`. */
    zvalue value;
} TokenInfo;

/**
 * Token structure.
 */
typedef struct {
    /** Value header. */
    DatValue header;

    /** Token info. */
    TokenInfo info;
} DatToken;

/**
 * Allocates memory, sized to include a `DatValue` header plus the
 * indicated number of extra bytes. The `DatValue` header is
 * initialized with the indicated type and size. The resulting value
 * is added to the stack.
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
 * Compares strings for equality. Only called when the sizes are the same.
 */
bool datStringEq(zvalue v1, zvalue v2);

/**
 * Compares strings for order.
 */
zorder datStringOrder(zvalue v1, zvalue v2);

/**
 * Compares lists for equality. Only called when the sizes are the same.
 */
bool datListEq(zvalue v1, zvalue v2);

/**
 * Compares lists for order.
 */
zorder datListOrder(zvalue v1, zvalue v2);

/**
 * Compares maps for equality. Only called when the sizes are the same.
 */
bool datMapEq(zvalue v1, zvalue v2);

/**
 * Compares maps for order.
 */
zorder datMapOrder(zvalue v1, zvalue v2);

/**
 * Compares uniqlets for order.
 */
zorder datUniqletOrder(zvalue v1, zvalue v2);

/**
 * Compares tokens for equality. Only called when the sizes are the same.
 */
bool datTokenEq(zvalue v1, zvalue v2);

/**
 * Compares tokens for order.
 */
zorder datTokenOrder(zvalue v1, zvalue v2);

/**
 * Marks list contents for garbage collection.
 */
void datListMark(zvalue value);

/**
 * Marks map contents for garbage collection.
 */
void datMapMark(zvalue value);

/**
 * Marks token contents for garbage collection.
 */
void datTokenMark(zvalue value);

/**
 * Marks uniqlet contents for garbage collection.
 */
void datUniqletMark(zvalue value);

/**
 * Frees uniqlet contents during garbage collection.
 */
void datUniqletFree(zvalue value);

#endif
