/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Exported functions
 */

/** Documented in API header. */
zcomparison samCompare(zvalue v1, zvalue v2) {
    ztype t1 = samType(v1);
    ztype t2 = samType(v2);

    if (v1 == v2) {
        return SAM_IS_EQUAL;
    } else if (t1 < t2) {
        return SAM_IS_LESS;
    } else if (t1 > t2) {
        return SAM_IS_MORE;
    }

    switch (t1) {
        case SAM_INTLET:  return samIntletCompare(v1, v2);
        case SAM_LISTLET: return samListletCompare(v1, v2);
        case SAM_MAPLET:  return samMapletCompare(v1, v2);
        case SAM_UNIQLET: return samUniqletCompare(v1, v2);
    }

    samDie("Invalid type (shouldn't happen).");
}
