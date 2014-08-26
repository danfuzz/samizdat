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
#include "zlimits.h"


enum {
    /** Whether to spew to the console during gc. */
    DAT_CHATTY_GC = false,

    /** Whether to be paranoid about values in collections / derived values. */
    DAT_CONSTRUCTION_PARANOIA = false,

    /** The class index for class `Builtin`. */
    DAT_INDEX_BUILTIN = 7,

    /** The class index for class `Jump`. */
    DAT_INDEX_JUMP = 8,

    /** The class index for class `Symbol`. */
    DAT_INDEX_SYMBOL = 2,

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

    /** Name of the class, as a symbol. */
    zvalue name;

    /** Access secret of the class. Optional, and arbitrary if present. */
    zvalue secret;

    /**
     * Class identifier / index. Assigned upon initialization, in sequential
     * order.
     */
    zint classId;

    /**
     * Bindings from method symbols to functions, keyed off of symbol
     * index number.
     */
    zvalue methods[DAT_MAX_SYMBOLS];
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
 * short-circuited method dispatch of `call` on class `Builtin`
 * lands.
 */
zvalue builtinCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Binds all the methods of a class. This is only supposed to be called from
 * the class initialization of classes that are partially built by the
 * object model bootstrap code. Everywhere else should use `makeClass()` or
 * `makeCoreClass()`.
 */
void classBindMethods(zvalue cls, zvalue classMethods, zvalue instanceMethods);

/**
 * Finds a method on a class, if bound. Returns the bound function if found
 * or `NULL` if not. Does not check to see if `index` is in the valid range
 * for a symbol index.
 */
zvalue classFindMethodBySymbolIndex(zvalue cls, zint index);

/**
 * Gets the index for a given class value. The given value *must* be a
 * `Class` per se; this is *not* checked.
 */
inline zint classIndexUnchecked(zvalue cls) {
    return ((ClassInfo *) datPayload(cls))->classId;
}

/**
 * Actual implementation of nonlocal jump calling. This is where
 * short-circuited method dispatch of `call` on class `Jump` lands.
 */
zvalue jumpCall(zvalue jump, zint argCount, const zvalue *args);

/**
 * Actual implementation of symbol calling. This is where
 * short-circuited method dispatch of `call` on class `Symbol`
 * lands. This calls the method bound to the given symbol, with the given
 * arguments. The method is looked up on `args[0]`. As such, `argCount` must
 * be at least `1`.
 */
zvalue symbolCall(zvalue symbol, zint argCount, const zvalue *args);

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
