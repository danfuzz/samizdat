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

    /** The type index for type `Builtin`. */
    PB_INDEX_BUILTIN = 2,

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

    /** Data type. This is always a `Type` instance. */
    zvalue type;

    /**
     * Unique identity number. Only set if requested, and only usable by
     * opaque types that are marked as `identified`.
     */
    zint identity;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} PbHeader;

/**
 * Payload struct for type `Type`.
 */
typedef struct {
    /** Parent type. Only allowed to be `NULL` for `Value`. */
    zvalue parent;

    /** Name of the type. Arbitrary value. */
    zvalue name;

    /** Access secret of the type. Optional, and arbitrary if present. */
    zvalue secret;

    /** Whether the type is derived. `false` indicates a core type. */
    bool derived : 1;

    /**
     * Whether the type is "identified". `true` indicates that
     * `valIdentityOf` will work on values of the type.
     */
    bool identified : 1;

    /**
     * Type identifier / index. Assigned upon initialization, in sequential
     * order.
     */
    zint id;
} TypeInfo;


/**
 * Binds the standard methods for a derived type.
 */
void derivBind(zvalue type);

/**
 * Actual implementation of builtin function calling. This is where
 * short-circuited generic function dispatch of `call` on type `Builtin`
 * lands.
 */
zvalue builtinCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Actual implementation of generic function calling. This is where
 * short-circuited generic function dispatch of `call` on type `Generic`
 * lands.
 */
zvalue genericCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Gets the function bound to the given generic for the given type index,
 * if any. Returns `NULL` if there is no binding.
 */
zvalue genericFindByIndex(zvalue generic, zint index);

/**
 * Gets the index for a given type value. The given value *must* be a
 * `Type` per se; this is *not* checked.
 */
inline zint indexFromTrueType(zvalue type) {
    return ((TypeInfo *) pbPayload(type))->id;
}

/**
 * Marks all the references on the frame stack. Returns the number of
 * references marked.
 */
zint markFrameStack(void);

/**
 * Gets the `Type` per se for the given value.
 */
inline zvalue trueTypeOf(zvalue value) {
    return value->type;
}

/**
 * Gets the `Type` per se from a type (which may be the name of a transparent
 * derived type) and secret. Returns `NULL` if the type and secret don't
 * match.
 */
zvalue typeFromTypeAndSecret(zvalue typeOrName, zvalue secret);

/**
 * Returns true iff the given type is a derived type (whether opaque or
 * transparent). This works (returns `true`) if given a non-`Type` value.
 */
bool typeIsDerived(zvalue typeOrName);

/**
 * Checks whether the given value matches the secret of the given type.
 * `secret` may be passed as `NULL`. This works (treating it as a transparent
 * type) if given a non-`Type` value for `typeOrName`.
 */
bool typeSecretIs(zvalue typeOrName, zvalue secret);


#endif
