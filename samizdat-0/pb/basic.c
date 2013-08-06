/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** The next "order id" to return. */
static zint theNextOrderId = 0;


/*
 * Exported functions
 */

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
        die("Mismatched types: %s, %s",
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
void pbInit(void) {
    if (TYPE_Type != NULL) {
        return;
    }

    // The initialization of the type system has to come first, because all
    // the other initializers create types (that is, values of type `Type`).
    pbInitTypeSystem();

    // Next the generics have to be defined, and the early types' methods
    // need to be bound, so that the rest of the binding can operate properly.
    pbInitCoreGenerics();
    pbBindGeneric();
    pbBindString();
    pbBindType();

    pbBindDeriv();
    pbBindFunction();
    pbBindInt();
}

/* Documented in header. */
zint pbOrderId(void) {
    if (theNextOrderId < 0) {
        die("Too many order ids!");
    }

    zint result = theNextOrderId;
    theNextOrderId++;
    return result;
}
