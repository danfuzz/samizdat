// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Uniqlet.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue makeUniqlet(void) {
    return datAllocValue(CLS_Uniqlet, 0);
}


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(Uniqlet) {
    MOD_USE(Value);

    CLS_Uniqlet = makeCoreClass("Uniqlet", CLS_Value,
        NULL,
        NULL);
}

// Documented in header.
zvalue CLS_Uniqlet = NULL;
