/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Exported functions
 */

/* Documented in header. */
void pbAssertFunction(zvalue value) {
    pbAssertType(value, DAT_Function);
}

/* Documented in header. */
void pbAssertGeneric(zvalue value) {
    pbAssertType(value, DAT_Generic);
}

/* Documented in header. */
void pbAssertInt(zvalue value) {
    pbAssertType(value, DAT_Int);
}

/* Documented in header. */
void pbAssertNth(zint size, zint n) {
    if (n < 0) {
        die("Invalid index (negative): %lld", n);
    }

    if (n >= size) {
        die("Invalid index: %lld; size %lld", n, size);
    }
}

/* Documented in header. */
void pbAssertNthOrSize(zint size, zint n) {
    if (n != size) {
        pbAssertNth(size, n);
    }
}

/* Documented in header. */
void pbAssertSameType(zvalue v1, zvalue v2) {
    pbAssertValid(v1);
    pbAssertValid(v2);

    if (v1->type != v2->type) {
        die("Mismatched core types: %s, %s",
            pbDebugString(v1), pbDebugString(v2));
    }
}

/* Documented in header. */
void pbAssertSliceRange(zint size, zint start, zint end) {
    if ((start < 0) || (end < 0) || (end < start)) {
        die("Invalid slice range: (%lld..!%lld)", start, end);
    }

    pbAssertNthOrSize(size, end);
}

/* Documented in header. */
void pbAssertString(zvalue value) {
    pbAssertType(value, DAT_String);
}

/* Documented in header. */
void pbAssertStringSize1(zvalue value) {
    pbAssertString(value);
    if (pbSize(value) != 1) {
        die("Not a size 1 string.");
    }
}

/* Documented in header. */
void pbAssertType(zvalue value, ztype type) {
    pbAssertValid(value);

    if (value->type != type) {
        die("Expected type %s; got %s.", type->name, pbDebugString(value));
    }
}

/* Documented in header. */
void pbAssertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (value->magic != PB_VALUE_MAGIC) {
        die("Invalid value (incorrect magic): %p", value);
    }

    if (value->type == NULL) {
        die("Invalid value (null type): %p", value);
    }
}
