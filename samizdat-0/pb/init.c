/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"
#include "type/Type.h"


/*
 * Exported Definitions
 */

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

    pbBindValue();      // This defines the universal cross-type generics.
    pbBindCallable();   // This defines the callable (function-like) generics.
    pbBindCollection(); // This defines the collection generics.

    pbBindType();
    pbBindString();
    pbBindFunction();
    pbBindGeneric();
    pbBindInt();
    pbBindUniqlet();
    pbBindList();
}
