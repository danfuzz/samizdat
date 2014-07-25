// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include "dat.h"
#include "util.h"


enum {
    /** Whether to spew to the console during gc. */
    DAT_CHATTY_GC = false,

    /** The class index for class `Builtin`. */
    DAT_INDEX_BUILTIN = 4,

    /** The class index for class `Generic`. */
    DAT_INDEX_GENERIC = 5,

    /** The class index for class `Jump`. */
    DAT_INDEX_JUMP = 6,

    /** Whether to be paranoid about corruption checks. */
    DAT_MEMORY_PARANOIA = false,

    /** "Magic number" for value validation. */
    DAT_VALUE_MAGIC = 0x600f1e57,

    /** Required byte alignment for values. */
    DAT_VALUE_ALIGNMENT = sizeof(zint)
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

    /** Class of the value. This is always a `Class` instance. */
    zvalue cls;

    /** Class-specific data goes here. */
    uint8_t payload[/*flexible*/];
} DatHeader;

/**
 * Payload struct for class `Class`.
 */
typedef struct {
    /** Parent class. Only allowed to be `NULL` for `Value`. */
    zvalue parent;

    /** Name of the class. Arbitrary value. */
    zvalue name;

    /** Access secret of the class. Optional, and arbitrary if present. */
    zvalue secret;

    /**
     * Class identifier / index. Assigned upon initialization, in sequential
     * order.
     */
    zint classId;
} ClassInfo;

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
 * Actual implementation of builtin function calling. This is where
 * short-circuited generic function dispatch of `call` on class `Builtin`
 * lands.
 */
zvalue builtinCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Gets the index for a given class value. The given value *must* be a
 * `Class` per se; this is *not* checked.
 */
inline zint classIndexUnchecked(zvalue cls) {
    return ((ClassInfo *) datPayload(cls))->classId;
}

/**
 * Actual implementation of generic function calling. This is where
 * short-circuited generic function dispatch of `call` on class `Generic`
 * lands.
 */
zvalue genericCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Gets the function bound to the given generic for the given class index,
 * if any. Returns `NULL` if there is no binding.
 */
zvalue genericFindByIndex(zvalue generic, zint index);

/**
 * Actual implementation of nonlocal jump calling. This is where
 * short-circuited generic function dispatch of `call` on class `Jump`
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


#endif
