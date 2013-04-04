/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"


/*
 * Helper functions
 */

/**
 * Compares intlets.
 */
static zcomparison compareIntlets(zvalue v1, zvalue v2) {
    bool neg1 = samIntletSign(v1);
    bool neg2 = samIntletSign(v2);

    if (neg1 != neg2) {
        return neg1 ? SAM_IS_LESS : SAM_IS_MORE;
    }

    // At this point, we know the two numbers are the same sign,
    // which makes it okay to do unsigned comparison (because, in
    // particular, the unsigned interpretations of two negative numbers
    // sort the same as the corresponding negative numbers).

    zint sz1 = samSize(v1);
    zint sz2 = samSize(v2);
    zint sz = sz1 > sz2 ? sz1 : sz2;

    for (zint i = sz - 1; i >= 0; i--) {
        zint n1 = samIntletGetInt(v1, i);
        zint n2 = samIntletGetInt(v2, i);
        if (n1 < n2) {
            return SAM_IS_LESS;
        } else if (n1 > n2) {
            return SAM_IS_MORE;
        }
    }

    return SAM_IS_EQUAL;
}

/**
 * Compares listlets.
 */
static zcomparison compareListlets(SamListlet *v1, SamListlet *v2) {
    zvalue *e1 = v1->elems;
    zvalue *e2 = v2->elems;
    zint sz1 = v1->header.size;
    zint sz2 = v2->header.size;
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i], e2[i]);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    return (sz1 < sz2) ? SAM_IS_LESS : SAM_IS_MORE;
}

/**
 * Compares maplets.
 */
static zcomparison compareMaplets(SamMaplet *v1, SamMaplet *v2) {
    zmapping *e1 = v1->elems;
    zmapping *e2 = v2->elems;
    zint sz1 = v1->header.size;
    zint sz2 = v2->header.size;
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i].key, e2[i].key);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    if (sz1 < sz2) {
        return SAM_IS_LESS;
    } else if (sz1 > sz2) {
        return SAM_IS_MORE;
    }

    for (zint i = 0; i < sz; i++) {
        zcomparison result = samCompare(e1[i].value, e2[i].value);
        if (result != SAM_IS_EQUAL) {
            return result;
        }
    }

    return SAM_IS_EQUAL;
}

/**
 * Compares uniqlets.
 */
static zcomparison compareUniqlets(SamUniqlet *v1, SamUniqlet *v2) {
    zint id1 = v1->id;
    zint id2 = v2->id;

    if (id1 < id2) {
        return SAM_IS_LESS;
    } else if (id1 > id2) {
        return SAM_IS_MORE;
    } else {
        return SAM_IS_EQUAL;
    }
}


/*
 * API implementation
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
        case SAM_INTLET: {
            return compareIntlets(v1, v2);
        }
        case SAM_LISTLET: {
            return compareListlets((SamListlet *) v1, (SamListlet *) v2);
        }
        case SAM_MAPLET: {
            return compareMaplets((SamMaplet *) v1, (SamMaplet *) v2);
        }
        case SAM_UNIQLET: {
            return compareUniqlets((SamUniqlet *) v1, (SamUniqlet *) v2);
        }
    }

    samDie("Invalid type (shouldn't happen).");
}
