// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include "cls.h"
#include "util.h"


enum {
    /** Whether to spew to the console about map cache hits. */
    CLS_CHATTY_LOOKUP_CACHE = false,

    /** Whether to be paranoid about values in collections / records. */
    CLS_CONSTRUCTION_PARANOIA = false,

    /**
     * Number of entries in the lookup cache. Probably best for this to be a
     * prime number (to get better distribution of cache elements). In
     * practice, it looks like the theoretical best case is probably about
     * 99.6% (that is, nearly every lookup is for a container/key pair that
     * have been observed before). The size of the cache is chosen to hit the
     * point of diminishing returns.
     */
    CLS_LOOKUP_CACHE_SIZE = 6007,

    /**
     * Maximum number of items that can be `collect`ed or `filter`ed out
     * of a generator, period.
     */
    CLS_MAX_GENERATOR_ITEMS_HARD = 50000,

    /**
     * Maximum number of items that can be `collect`ed or `filter`ed out
     * of a generator, without resorting to heavyweight memory operations.
     */
    CLS_MAX_GENERATOR_ITEMS_SOFT = 1000
};

#endif
