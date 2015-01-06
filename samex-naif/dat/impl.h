// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

    /** Whether to be paranoid about values in collections / records. */
    DAT_CONSTRUCTION_PARANOIA = false,

    /** Largest code point to keep a cached single-character string for. */
    DAT_MAX_CACHED_CHAR = 127,

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

    /** Required byte alignment for values. */
    DAT_VALUE_ALIGNMENT = sizeof(zint)
};

/**
 * "Colors" used for gc marking. This is used instead of a boolean so that
 * we can flip the sense of marked-vs-not without actually having to flip a
 * bunch of bits in live objects.
 */
typedef enum {
    MARK_AZURE,
    MARK_MAUVE
} zmarkColor;

/**
 * Common fields across all values. Used as a header for other types.
 *
 * **Note:** This must match the definition of `DatHeaderExposed` in `dat.h`.
 */
typedef struct DatHeader {
    /**
     * Forward circular link. Every value is linked into a circularly
     * linked list, which identifies its current fate / classification.
     */
    zvalue next;

    /** Backward circular link. */
    zvalue prev;

    /** Class of the value. This is always a `Class` instance. */
    zvalue cls;

    /** Mark bit (used during GC). */
    zmarkColor mark : 1;

    /** Class-specific data goes here. */
    void *payload[/*flexible*/];
} DatHeader;


/**
 * Implementation of method `Builtin.call()`. This is used in the code
 * for `methCall()` to avoid infinite recursion. **Note:** Assumes that
 * `function` is in fact an instance of `Builtin`.
 */
zvalue builtinCall(zvalue function, zarray args);

/**
 * Short-circuit to call the `.gcMark()` method on `value`, if it has one.
 * Does nothing if not.
 */
void callGcMark(zvalue value);

/**
 * Binds all the methods of a class. Either `*Methods` argument can be
 * `NULL`, in which case it is treated as `@{}` (the empty symbol table).
 *
 * This is only supposed to be called from the class initialization of classes
 * that are partially built by the object model bootstrap code. Everywhere
 * else should use `makeClass()` or `makeCoreClass()`.
 */
void classBindMethods(zvalue cls, zvalue classMethods, zvalue instanceMethods);

/**
 * Finds a method on a class, if bound. Returns the bound function if found
 * or `NULL` if not. Does not check to see if `cls` is actually a class,
 * and does not check if `index` is in the valid range for a symbol index.
 */
zvalue classFindMethodUnchecked(zvalue cls, zint index);

/**
 * Marks all the references on the frame stack. Returns the number of
 * references marked.
 */
zint markFrameStack(void);

/**
 * Gets the value for the given symbol key in the given symbol table.
 * Does not check to see if `symtab` is in fact a symbol table.
 */
zvalue symtabGetUnchecked(zvalue symtab, zvalue key);


//
// Object model initialization. These functions are needed in order to
// keep the implementations of the "corest of the core" classes in separate
// files while still allowing them to be initialized in two steps, as needed
// due to the circular nature of the class structure.
//

void bindMethodsForBuiltin(void);
void bindMethodsForClass(void);
void bindMethodsForCore(void);
void bindMethodsForSymbol(void);
void bindMethodsForSymbolTable(void);
void bindMethodsForValue(void);
void initCoreSymbols(void);

#endif
