// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// A grab-bag of various implementation limits / limitations and
// generally tweakable implementation constants. The prefix on each
// value's name indicates which module "owns" it.
//

#ifndef _ZLIMITS_H_
#define _ZLIMITS_H_

enum {
    /** Number of allocations between each forced gc. */
    DAT_ALLOCATIONS_PER_GC = 500000,

    /**
     * Number of entries in the map lookup cache. Probably best for this
     * to be a prime number (to get better distribution of cache elements).
     */
    DAT_MAP_CACHE_SIZE = 70001,

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
    DAT_MAX_IMMORTALS = 4000,

    /** Maximum number of references on the stack. */
    DAT_MAX_STACK = 100000,

    /**
     * Maximum size in characters of a string that can be handled
     * on the stack, without resorting to heavyweight memory operations.
     */
    DAT_MAX_STRING_SOFT = 10000,

    /** Maximum number of classes allowed. */
    DAT_MAX_CLASSES = 2500,

    /** Maximum number of method symbols allowed. */
    DAT_MAX_SYMBOLS = 500,

    /** Maximum (highest value) small int constant to keep. */
    DAT_SMALL_INT_MAX = 700,

    /** Minumum (lowest value) small int constant to keep. */
    DAT_SMALL_INT_MIN = -300,

    /** Maximum readable file size, in bytes. */
    IO_MAX_FILE_SIZE = 100000,

    /** Maximum number of formal arguments to a function. */
    LANG_MAX_FORMALS = 10,

    /**
     * Maximum number of tokens in a given parse (which is the maximum
     * number of characters for a tokenizer).
     */
    LANG_MAX_TOKENS = 100000,

    /**
     * Maximum number of characters in a tokenized string constant,
     * identifier, or directive.
     */
    LANG_MAX_STRING_CHARS = 200,

    /** Maximum number of active stack frames. */
    UTIL_MAX_CALL_STACK_DEPTH = 4000,

    /** Maximum number of disjoint heap allocation page ranges. */
    UTIL_MAX_PAGE_RANGES = 400
};

#endif
