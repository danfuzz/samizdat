/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"


/*
 * Exported Definitions
 */

/** Initializes the module. */
MOD_INIT(pb) {
    // It suffices to just init the concrete types and `Deriv`. Everything
    // else flows out of those.

    // Concrete types.
    MOD_USE(Value);
}
