/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "alloc.h"
#include "impl.h"
#include "unicode.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions
 */

/** The empty listlet, lazily initialized. */
static zvalue theEmptyListlet = NULL;


/*
 * Intra-library API implementation
 */

/** Documented in `impl.h`. */
zvalue samAllocListlet(zint size) {
    return samAllocValue(SAM_LISTLET, size, size * sizeof(zvalue));
}

/** Documented in `impl.h`. */
zvalue *samListletElems(zvalue listlet) {
    samAssertListlet(listlet);

    return ((SamListlet *) listlet)->elems;
}

/** Documented in `impl.h`. */
zcomparison samListletCompare(zvalue v1, zvalue v2) {
    zvalue *e1 = samListletElems(v1);
    zvalue *e2 = samListletElems(v2);
    zint sz1 = samSize(v1);
    zint sz2 = samSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i], e2[i]);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    return (sz1 < sz2) ? SAM_IS_LESS : SAM_IS_MORE;
}


/*
 * API Implementation
 */

/** Documented in API header. */
zvalue samListletGet(zvalue listlet, zint n) {
    samAssertNth(listlet, n);
    return samListletElems(listlet)[n];
}

/** Documented in API header. */
zvalue samListletEmpty(void) {
    if (theEmptyListlet == NULL) {
        theEmptyListlet = samAllocListlet(0);
    }

    return theEmptyListlet;
}

/** Documented in API header. */
zvalue samListletAppend(zvalue listlet, zvalue value) {
    samAssertListlet(listlet);

    zint oldSize = listlet->size;
    zint size = oldSize + 1;
    zvalue result = samAllocListlet(size);
    zvalue *resultElems = samListletElems(result);

    memcpy(resultElems, samListletElems(listlet), oldSize * sizeof(zvalue));
    resultElems[oldSize] = value;

    return result;
}
