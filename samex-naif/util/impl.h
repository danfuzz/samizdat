// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include "util.h"

enum {
    /** Initial buffer size for string formatting. */
    UTIL_INITIAL_FORMAT_SIZE = 200,

    /** Maximum number of active stack frames. */
    UTIL_MAX_CALL_STACK_DEPTH = 4000,

    /** Maximum number of disjoint heap allocation page ranges. */
    UTIL_MAX_PAGE_RANGES = 400
};

#endif
