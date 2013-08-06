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
    PB_VALUE_MAGIC = 0x600f1e57,

    /** Required byte alignment for values. */
    PB_VALUE_ALIGNMENT = 8
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
    zvalue type;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} PbHeader;

/**
 * Gets the function bound to the given generic for the given value, if any.
 * Returns `NULL` if there is no binding.
 */
zfunction gfnFind(zvalue generic, zvalue value);

/**
 * Gets the index for a given type value.
 */
zint indexFromType(zvalue type);

/**
 * Like `pbAllocValue`, except that the `type` of the result is set to itself.
 * This is only used during initial bootstrap, to allocate the type `Type`.
 */
zvalue pbAllocTypeType(zint extraBytes);


/*
 * Initialization functions
 */

/**
 * Initializes the type system, including in particular the type value
 * `Type`. This also creates the type `String` but doesn't bind it; it's just
 * enough so that types can be given string names.
 */
void pbInitTypeSystem(void);

/**
 * Initializes the core generic functions.
 */
void pbInitCoreGenerics(void);

// Per-type binding and initialization.
void pbBindDeriv(void);
void pbBindFunction(void);
void pbBindGeneric(void);
void pbBindInt(void);
void pbBindString(void);
void pbBindType(void);

#endif
