// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


#include <stdlib.h>

#include "util.h"


//
// Exported Definitions
//

// Documented in header.
void utilSortStable(void *base, size_t nel, size_t width,
        int (*compar)(const void *, const void *)) {
    // TODO: An actual implementation, when `mergesort` isn't available in
    // the standard library.
    mergesort(base, nel, width, compar);
}
