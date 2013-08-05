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

#include "pb.h"
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
typedef struct PbHeader {
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
} PbHeader;

/**
 * Gets the function bound to the given generic for the given value, if any.
 * Returns `NULL` if there is no binding.
 */
zfunction datGfnFind(zvalue generic, zvalue value);

/**
 * Gets the sequence number index for a `ztype`, initializing it if necessary.
 */
zint datIndexFromType(ztype type);

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
void pbInitCoreGenerics(void);

// Per-type generic binding.
void pbBindDeriv(void);
void pbBindFunction(void);
void pbBindGeneric(void);
void pbBindInt(void);
void pbBindString(void);

#endif
