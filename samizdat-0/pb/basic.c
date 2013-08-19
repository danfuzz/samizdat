/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Asserts and Initialization
 */

#include "impl.h"
#include "type/Type.h"


/*
 * Exported Definitions
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
    // This also initializes type values for all the types that `Type`
    // depends on.
    pbInitTypeSystem();

    pbBindValue(); // This defines the cross-type generics.
    pbBindType();
    pbBindString();

    pbBindCallable(); // This defines the callable generics.
    pbBindFunction();
    pbBindGeneric();

    pbBindInt();
    pbBindUniqlet();
}
