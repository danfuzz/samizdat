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
    bool marked : 1;

    /** Data type. */
    zvalue type;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} PbHeader;

/**
 * Payload data for all Deriv values.
 */
typedef struct {
    /** Data payload. */
    zvalue data;
} DerivInfo;

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
 * Like `pbAllocValue`, except that no checking of `type` is done.
 * This is only used during initial bootstrap, to allocate the
 * types `Type` and `String` (which have reference cycles).
 */
zvalue pbAllocValueUnchecked(zvalue type, zint extraBytes);

/**
 * Gets a transparent derived type, given its name. This creates the type
 * if necessary.
 */
zvalue transparentTypeFromName(zvalue name);

/**
 * Returns true iff the given type is a derived type (whether opaque or
 * transparent).
 */
bool typeIsDerived(zvalue type);

/**
 * Checks whether the given value matches the secret of the given type.
 * `secret` may be passed as `NULL`.
 */
bool typeSecretIs(zvalue type, zvalue secret);


/*
 * Method bindings for derived types.
 */

zvalue Deriv_dataOf(zvalue state, zint argCount, const zvalue *args);
zvalue Deriv_eq(zvalue state, zint argCount, const zvalue *args);
zvalue Deriv_gcMark(zvalue state, zint argCount, const zvalue *args);
zvalue Deriv_order(zvalue state, zint argCount, const zvalue *args);


/*
 * Initialization functions
 */

/**
 * Initializes the type system, including in particular the type value
 * `Type`. This also creates the types `String` and `Generic` but doesn't
 * bind methods to them; it's just enough so that types can be given names
 * and generics can be defined.
 */
void pbInitTypeSystem(void);

/**
 * Initializes the core generic functions.
 */
void pbInitCoreGenerics(void);

// Per-type binding and initialization.
void pbBindFunction(void);
void pbBindGeneric(void);
void pbBindInt(void);
void pbBindString(void);
void pbBindType(void);

#endif
