/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Asserts a particular type.
 */
static void assertType(zvalue value, ztype type) {
    datAssertValid(value);

    if (value->type != type) {
        die("Expected type %s: (%p)->type == %s",
            type->name, value, value->type->name);
    }
}


/*
 * Module functions
 */

/* Documented in header. */
void datAssertNth(zvalue value, zint n) {
    datAssertValid(value);

    if (!datHasNth(value, n)) {
        die("Invalid index: %lld; size %lld", n, datSize(value));
    }
}

/* Documented in header. */
void datAssertNthOrSize(zvalue value, zint n) {
    if ((n != datSize(value)) && !datHasNth(value, n)) {
        die("Invalid index: %lld; size %lld", n, datSize(value));
    }
}

/* Documented in header. */
void datAssertSliceRange(zvalue value, zint start, zint end) {
    if ((start < 0) || (end < 0) || (end < start)) {
        die("Invalid slice range: (%lld..!%lld)", start, end);
    }

    datAssertNthOrSize(value, end);
}

/* Documented in header. */
bool datHasNth(zvalue value, zint n) {
    return (n >= 0) && (n < datSize(value));
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertInt(zvalue value) {
    assertType(value, DAT_Int);
}

/* Documented in header. */
void datAssertString(zvalue value) {
    assertType(value, DAT_String);
}

/* Documented in header. */
void datAssertStringSize1(zvalue value) {
    datAssertString(value);
    if (datSize(value) != 1) {
        die("Not a size 1 string.");
    }
}

/* Documented in header. */
void datAssertList(zvalue value) {
    assertType(value, DAT_List);
}

/* Documented in header. */
void datAssertMap(zvalue value) {
    assertType(value, DAT_Map);
}

/* Documented in header. */
void datAssertMapSize1(zvalue value) {
    datAssertMap(value);
    if (datSize(value) != 1) {
        die("Not a size 1 map.");
    }
}

/* Documented in header. */
void datAssertUniqlet(zvalue value) {
    assertType(value, DAT_Uniqlet);
}

/* Documented in header. */
void datAssertDeriv(zvalue value) {
    assertType(value, DAT_Deriv);
}

/* Documented in header. */
zint datSize(zvalue value) {
    return value->size;
}

/* Documented in header. */
ztypeId datType(zvalue value) {
    return value->type->id;
}

/* Documented in header. */
bool datTypeIs(zvalue value, ztype type) {
    return value->type == type;
}
