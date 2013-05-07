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
 * If the given pointer is reasonably believed to be a zvalue, returns it;
 * otherwise returns `NULL`. This is used for conservative stack scanning.
 */
zvalue datConservativeValueCast(void *maybeValue);

/**
 * Returns whether the given value (which must be valid) has an
 * `n`th element, according to its defined size. This is only
 * useful with some types.
 */
bool datHasNth(zvalue value, zint n);

/**
 * Compares intlets for equality. Only called when the sizes are the same.
 */
bool datIntletEq(zvalue v1, zvalue v2);

/**
 * Compares intlets for order.
 */
zorder datIntletOrder(zvalue v1, zvalue v2);

/**
 * Compares stringlets for equality. Only called when the sizes are the same.
 */
bool datStringletEq(zvalue v1, zvalue v2);

/**
 * Compares stringlets for order.
 */
zorder datStringletOrder(zvalue v1, zvalue v2);

/**
 * Compares listlets for equality. Only called when the sizes are the same.
 */
bool datListletEq(zvalue v1, zvalue v2);

/**
 * Compares listlets for order.
 */
zorder datListletOrder(zvalue v1, zvalue v2);

/**
 * Compares maplets for equality. Only called when the sizes are the same.
 */
bool datMapletEq(zvalue v1, zvalue v2);

/**
 * Compares maplets for order.
 */
zorder datMapletOrder(zvalue v1, zvalue v2);

/**
 * Compares uniqlets for order.
 */
zorder datUniqletOrder(zvalue v1, zvalue v2);

/**
 * Compares highlets for equality. Only called when the sizes are the same.
 */
bool datHighletEq(zvalue v1, zvalue v2);

/**
 * Compares highlets for order.
 */
zorder datHighletOrder(zvalue v1, zvalue v2);

/**
 * Marks listlet contents for garbage collection.
 */
void datListletMark(zvalue value);

/**
 * Marks maplet contents for garbage collection.
 */
void datMapletMark(zvalue value);

/**
 * Marks highlet contents for garbage collection.
 */
void datHighletMark(zvalue value);

/**
 * Marks uniqlet contents for garbage collection.
 */
void datUniqletMark(zvalue value);

#endif
