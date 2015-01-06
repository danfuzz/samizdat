// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
    /** Whether to be paranoid about values in collections / records. */
    CLS_CONSTRUCTION_PARANOIA = false,

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
