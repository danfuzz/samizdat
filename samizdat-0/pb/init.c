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
    MOD_USE(typeSystem);

    MOD_USE(Value); // This defines the universal cross-type generics.

    // Protocols.
    MOD_USE(Bitwise);
    MOD_USE(Collection);
    MOD_USE(Function);
    MOD_USE(Number);
    MOD_USE(OneOff);

    // Normal types.
    MOD_USE(Type);
    MOD_USE(String);
    MOD_USE(Builtin);
    MOD_USE(Generic);
    MOD_USE(Int);
    MOD_USE(Uniqlet);
    MOD_USE(List);

    // Used for all derived types.
    MOD_USE(Deriv);
}
