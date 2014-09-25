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
    /** Number of allocations between each forced gc. */
    DAT_ALLOCATIONS_PER_GC = 500000,

    /** Whether to spew to the console during gc. */
    DAT_CHATTY_GC = false,

    /** Whether to spew to the console about map cache hits. */
    DAT_CHATTY_MAP_CACHE = false,

    /** Whether to be paranoid about values in collections / records. */
    DAT_CONSTRUCTION_PARANOIA = false,

    /**
     * Number of entries in the map lookup cache. Probably best for this
     * to be a prime number (to get better distribution of cache elements).
     * In practice it looks like the theoretical best case is probably about
     * 99.6% (that is, nearly every lookup is for a map/key pair that have
     * been observed before). The size of the map cache is chosen to hit the
     * point of diminishing returns.
     */
    DAT_MAP_CACHE_SIZE = 6007,

    /** Largest code point to keep a cached single-character string for. */
    DAT_MAX_CACHED_CHAR = 127,

    /**
     * Maximum number of items that can be `collect`ed or `filter`ed out
     * of a generator, period.
     */
    DAT_MAX_GENERATOR_ITEMS_HARD = 50000,

    /**
     * Maximum number of items that can be `collect`ed or `filter`ed out
     * of a generator, without resorting to heavyweight memory operations.
     */
    DAT_MAX_GENERATOR_ITEMS_SOFT = 1000,

    /** Maximum number of immortal values allowed. */
    DAT_MAX_IMMORTALS = 10000,

    /** Maximum number of references on the stack. */
    DAT_MAX_STACK = 100000,

    /**
     * Maximum size in characters of a string that can be handled
     * on the stack, without resorting to heavyweight memory operations.
     */
    DAT_MAX_STRING_SOFT = 10000,

    /** Maximum size in characters of a symbol name. */
    DAT_MAX_SYMBOL_SIZE = 80,

    /** Whether to be paranoid about corruption checks. */
    DAT_MEMORY_PARANOIA = false,

    /** Maximum (highest value) small int constant to keep. */
    DAT_SMALL_INT_MAX = 700,

    /** Minumum (lowest value) small int constant to keep. */
    DAT_SMALL_INT_MIN = -300,

    /**
     * Maximum number of probes allowed before using a larger symbol
     * table backing array.
     */
    DAT_SYMTAB_MAX_PROBES = 4,

    /** Minimum size of a symbol table backing array. */
    DAT_SYMTAB_MIN_SIZE = 10,

    /** Scaling factor when growing a symbol table backing array. */
    DAT_SYMTAB_SCALE_FACTOR = 2,

    /** "Magic number" for value validation. */
    DAT_VALUE_MAGIC = 0x600f1e57,

    /** Required byte alignment for values. */
    DAT_VALUE_ALIGNMENT = sizeof(zint)
};

/**
 * Common fields across all values. Used as a header for other types.
 *
 * **Note:** This must match the definition of `DatPartialHeader` in `dat.h`.
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
     * Whether this class has any subclasses. If so, it's invalid to
     * add any method bindings. TODO: Remove this once incremental method
     * binding is no longer allowed at all.
     */
    bool hasSubclasses;

    /**
     * Bindings from method symbols to functions, keyed off of symbol
     * index number.
     */
    zvalue methods[DAT_MAX_SYMBOLS];
} ClassInfo;

/**
 * Entry in the map cache. The cache is used to speed up calls to `mapFind`
 * (see which for details).
 */
typedef struct {
    /** Map to look up a key in. */
    zvalue map;

    /** Key to look up. */
    zvalue key;

    /** Result from `mapFind`. */
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
 * or `NULL` if not. Does not check to see if `cls` is actually a class,
 * and does not check if `index` is in the valid range for a symbol index.
 */
zvalue classFindMethodUnchecked(zvalue cls, zint index);

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
