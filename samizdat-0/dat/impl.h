/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Private implementation details
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
    /** Simple string name for the type. */
    const char *name;

    /**
     * In-model string value corresponding to `name` (above). Lazily
     * initialized.
     */
    zvalue nameValue;

    /** Type sequence number, complemented to disambiguate `0`. */
    zint seqNumCompl;

    /**
     * Gets the (overt) type of a value of the given type. Optional (may
     * be `NULL`), and if omitted means that the low-layer type name
     * is used.
     */
    zvalue (*typeOf)(zvalue);

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
     * ever called when the two values are not `==`. Optional (may be `NULL`),
     * and if omitted means that comparisons when not `==` are always false.
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
 * `dataOf(value)`: Gets the data payload of a value of the given type,
 * if any. Defaults to returning the value itself as its own payload.
 */
extern zvalue genDataOf;

/**
 * `sizeOf(value)`: Gets the "size" of a value of the given type, for the
 * appropriate per-type meaning of size. Defaults to always returning `0`.
 */
extern zvalue genSizeOf;

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
 * Gets the sequence number index for a `ztype`, initializing it if necessary.
 */
zint datIndexFromType(ztype type);

/**
 * Clears the contents of the map lookup cache.
 */
void datMapClearCache(void);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
void *datPayload(zvalue value);

/**
 * Gets a type value from a `ztype`.
 */
zvalue datTypeFromZtype(ztype type);


/*
 * Initialization functions
 */

/**
 * Initializes the core generic functions.
 */
void datInitCoreGenerics(void);

// Per-type generic binding.
void datBindDeriv(void);
void datBindFunction(void);
void datBindGeneric(void);
void datBindInt(void);
void datBindList(void);
void datBindMap(void);
void datBindString(void);
void datBindUniqlet(void);

#endif
