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

    pbBindValue(); // This defines the universal cross-type generics.

    // Protocols.
    pbBindBitwise();
    pbBindCollection();
    pbBindFunction();
    pbBindNumber();
    pbBindOneOff();

    pbBindType();
    pbBindString();
    pbBindBuiltin();
    pbBindGeneric();
    pbBindInt();
    pbBindUniqlet();
    pbBindList();
    pbBindDeriv();
}
