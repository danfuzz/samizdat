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
void datAssertNth(zint size, zint n) {
    if (n < 0) {
        die("Invalid index (negative): %lld", n);
    }

    if (n >= size) {
        die("Invalid index: %lld; size %lld", n, size);
    }
}

/* Documented in header. */
void datAssertNthOrSize(zint size, zint n) {
    if (n != size) {
        datAssertNth(size, n);
    }
}

/* Documented in header. */
void datAssertSliceRange(zint size, zint start, zint end) {
    if ((start < 0) || (end < 0) || (end < start)) {
        die("Invalid slice range: (%lld..!%lld)", start, end);
    }

    datAssertNthOrSize(size, end);
}

/* Documented in header. */
void datAssertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (value->magic != DAT_VALUE_MAGIC) {
        die("Invalid value (incorrect magic): %p", value);
    }

    if (value->type == NULL) {
        die("Invalid value (null type): %p", value);
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertDeriv(zvalue value) {
    assertType(value, DAT_Deriv);
}

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
