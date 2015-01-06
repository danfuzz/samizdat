// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


#include "util.h"

#include "timsort/timsort.h"


//
// Exported Definitions
//

// Documented in header.
void utilSortStable(void *base, size_t nel, size_t width,
        int (*compar)(const void *, const void *)) {
    timsort(base, nel, width, compar);
}
