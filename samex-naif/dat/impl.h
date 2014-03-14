/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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
    DAT_VALUE_ALIGNMENT = 8,

    /** The type index for type `Builtin`. */
    DAT_INDEX_BUILTIN = 2,

    /** The type index for type `Generic`. */
    DAT_INDEX_GENERIC = 3,

    /** The type index for type `Jump`. */
    DAT_INDEX_JUMP = 4
};

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
    bool marked : 1;

    /** Data type. This is always a `Type` instance. */
    zvalue type;

    /**
     * Unique identity number. Only set if requested, and only usable by
     * opaque types that are marked as `selfish`.
     */
    zint identity;

    /** Type-specific data goes here. */
    uint8_t payload[/*flexible*/];
} DatHeader;

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
     * Whether the type is "selfish." `true` indicates that
     * `valIdentityOf` will work on values of the type.
     */
    bool selfish : 1;

    /**
     * Type identifier / index. Assigned upon initialization, in sequential
     * order.
     */
    zint id;
} TypeInfo;

/**
 * Entry in the map cache. The cache is used to speed up calls to `mapFind`.
 * In practice it looks like the theoretical best case is probably about
 * 71.75% (that is, nearly 3 of 4 lookups are for a map/key pair that have
 * been observed before). The size of the map cache is chosen to hit the
 * point of diminishing returns.
 */
typedef struct {
    /** Map to look up a key in. */
    zvalue map;

    /** Key to look up. */
    zvalue key;

    /** Result from `mapFind` (see which for details). */
    zint index;
} MapCacheEntry;


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
inline zint typeIndexUnchecked(zvalue type) {
    return ((TypeInfo *) datPayload(type))->id;
}

/**
 * Actual implementation of nonlocal jump calling. This is where
 * short-circuited generic function dispatch of `call` on type `Jump`
 * lands.
 */
zvalue jumpCall(zvalue jump, zint argCount, const zvalue *args);

/**
 * Gets the `CacheEntry` for the given map/key pair.
 */
MapCacheEntry *mapGetCacheEntry(zvalue map, zvalue key);

/**
 * Marks all the references on the frame stack. Returns the number of
 * references marked.
 */
zint markFrameStack(void);

/**
 * Checks whether the given value matches the secret of the given type.
 * `secret` may be passed as `NULL`.
 */
bool typeHasSecret(zvalue type, zvalue secret);


#endif
