/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * A grab-bag of various implementation limits / limitations and
 * generally tweakable implementation constants.
 */

#ifndef _ZLIMITS_H_
#define _ZLIMITS_H_

enum {
    /** Number of small int constants to keep as preallocated values. */
    CONST_SMALL_INT_COUNT = 1000,

    /** Minumum (lowest value) small int constant to keep. */
    CONST_SMALL_INT_MIN = -300,

    /** Number of allocations between each forced gc. */
    DAT_ALLOCATIONS_PER_GC = 500000,

    /** Maximum number of immortal values allowed. */
    DAT_MAX_IMMORTALS = 1500,

    /** Maximum number of references on the stack. */
    DAT_MAX_STACK = 50000,

    /** Maximum readable file size, in bytes. */
    IO_MAX_FILE_SIZE = 100000,

    /** Maximum number of tokens in a given parse. */
    LANG_MAX_TOKENS = 100000,

    /** Maximum number of characters in an identifier. */
    LANG_MAX_IDENTIFIER_CHARS = 40,

    /** Maximum number of characters in a tokenized string constant. */
    LANG_MAX_STRING_CHARS = 200,

    /** Maximum number of active stack frames. */
    UTIL_MAX_CALL_STACK_DEPTH = 2000,

    /** Maximum number of disjoint heap allocation page ranges. */
    UTIL_MAX_PAGE_RANGES = 200
};

#endif
