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
    } else if (v1->type != v2->type) {
        return false;
    } else {
        return v1->type->eq(v1, v2);
    }
}

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return ZSAME;
    } else if (v1->type == v2->type) {
        return v1->type->order(v1, v2);
    } else {
        return (v1->type->id < v2->type->id) ? ZLESS : ZMORE;
    }
}

/* Documented in header. */
zint datSize(zvalue value) {
    return value->type->sizeOf(value);
}

/* Documented in header. */
ztypeId datType(zvalue value) {
    return value->type->id;
}

/* Documented in header. */
bool datTypeIs(zvalue value, ztype type) {
    return value->type == type;
}
