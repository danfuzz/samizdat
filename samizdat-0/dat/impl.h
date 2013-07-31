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
     * per-type meaning of size. Optional (may be `NULL`), and if omitted
     * means that the size is always `0`.
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
 * Common fields across all values. Used as a header for other types.
 */
typedef struct DatHeader {
    /**
     * Forward circular link. Every value is linked into a circularly
     * linked list, which identifies its current fate / classification.
     */
    zvalue next;

    /** Backward circular link. */
    zvalue prev;

    /** Magic number (for sanity / validation checks). */
    uint32_t magic;

    /** Mark bit (used during GC). */
    bool marked;

    /** Data type. */
    ztype type;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} DatHeader;

/**
 * Flag indicating whether module has been initialized.
 */
extern bool datInitialized;

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
 * Asserts that the given value is a valid `zvalue` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void datAssertValid(zvalue value);

/**
 * Clears the contents of the map lookup cache.
 */
void datMapClearCache(void);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
void *datPayload(zvalue value);

#endif
