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
    PB_VALUE_ALIGNMENT = 8,

    /** The type index for type `Function`. */
    PB_INDEX_FUNCTION = 2,

    /** The type index for type `Generic`. */
    PB_INDEX_GENERIC = 3
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

    /**
     * Data type. This can be either a `Type` per se, or it can be an
     * arbitrary value to represent a transparent derived type. In the
     * latter case, it can later be replaced (as needed) with a real `Type`.
     */
    zvalue type;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} PbHeader;

/**
 * Binds the standard methods for a derived type.
 */
void derivBind(zvalue type);

/**
 * Actual implementation of normal function calling. This is where
 * short-circuited generic function dispatch of `call` on type `Function`
 * lands.
 */
zvalue functionCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Gets the function bound to the given generic for the given value, if any.
 * Returns `NULL` if there is no binding.
 */
zvalue genericFind(zvalue generic, zvalue value);

/**
 * Gets the index for a given type value. The given value *must* be a
 * `Type` per se.
 */
zint indexFromType(zvalue type);

/**
 * Gets a transparent derived type, given its name. This creates the type
 * if necessary.
 */
zvalue transparentTypeFromName(zvalue name);

/**
 * Gets the `Type` per se for the given value. This creates a lightweight
 * transparent type, updating the value, if necessary.
 */
zvalue trueTypeOf(zvalue value);

/**
 * Returns true iff the given type is a derived type (whether opaque or
 * transparent). This works (returns `true`) if given a non-`Type` value.
 */
bool typeIsDerived(zvalue type);

/**
 * Checks whether the given value matches the secret of the given type.
 * `secret` may be passed as `NULL`. This works (treating it as a transparent
 * type) if given a non-`Type` value for `type`.
 */
bool typeSecretIs(zvalue type, zvalue secret);


/*
 * Initialization functions
 */

/**
 * Initializes the type system, including in particular the type values
 * `Type` and `Value`. This also creates the types `String` and `Generic` but
 * doesn't bind methods for them; it's just enough so that types can be given
 * names and generics can be defined.
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
void pbBindValue(void);

#endif
