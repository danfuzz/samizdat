/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * A grab-bag of various implementation limits / limitations and
 * generally tweakable implementation constants. The prefix on each
 * value's name indicates which module "owns" it.
 */

#ifndef _ZLIMITS_H_
#define _ZLIMITS_H_

enum {
    /** Maximum number of items that can be interpolated out of a generator. */
    CONST_MAX_GENERATOR_ITEMS = 10000,

    /**
     * Number of entries in the map lookup cache. Probably best for this
     * to be a prime number (to get better distribution of cache elements).
     */
    DAT_MAP_CACHE_SIZE = 70001,

    /** Maximum readable file size, in bytes. */
    IO_MAX_FILE_SIZE = 100000,

    /** Maximum number of tokens in a given parse. */
    LANG_MAX_TOKENS = 100000,

    /** Maximum number of characters in an identifier. */
    LANG_MAX_IDENTIFIER_CHARS = 40,

    /** Maximum number of characters in a tokenized string constant. */
    LANG_MAX_STRING_CHARS = 200,

    /** Number of allocations between each forced gc. */
    PB_ALLOCATIONS_PER_GC = 500000,

    /** Maximum number of immortal values allowed. */
    PB_MAX_IMMORTALS = 1500,

    /** Maximum number of references on the stack. */
    PB_MAX_STACK = 50000,

    /** Maximum number of types allowed. */
    PB_MAX_TYPES = 25,

    /** Number of small int constants to keep as preallocated values. */
    PB_SMALL_INT_COUNT = 1000,

    /** Minumum (lowest value) small int constant to keep. */
    PB_SMALL_INT_MIN = -300,

    /** Maximum number of active stack frames. */
    UTIL_MAX_CALL_STACK_DEPTH = 4000,

    /** Maximum number of disjoint heap allocation page ranges. */
    UTIL_MAX_PAGE_RANGES = 400
};

#endif
