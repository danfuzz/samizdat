/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"


/*
 * Exported functions
 */

/* Documented in header. */
bool datEq(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return true;
    }

    if (v1->type != v2->type) {
        return false;
    }

    return v1->type->eq(v1, v2);
}

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return ZSAME;
    }

    ztypeId t1 = datType(v1);
    ztypeId t2 = datType(v2);

    if (t1 < t2) {
        return ZLESS;
    } else if (t1 > t2) {
        return ZMORE;
    }

    return v1->type->order(v1, v2);
}
