// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Uniqlet.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue makeUniqlet(void) {
    return datAllocValue(TYPE_Uniqlet, 0);
}


//
// Type Definition
//

/** Initializes the module. */
MOD_INIT(Uniqlet) {
    MOD_USE(Value);

    // Note: The `typeSystem` module initializes `TYPE_Uniqlet`.

    // This function is just left here as a positive indication that there
    // is nothing to do.
}

// Documented in header.
zvalue TYPE_Uniqlet = NULL;
