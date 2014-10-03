// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Core` class
//

#include "type/Core.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(Core) {
    MOD_USE(Value);

    // Note: The `objectModel` module initializes `CLS_Core`.
    classBindMethods(CLS_Core,
        NULL,
        NULL);
}

// Documented in header.
zvalue CLS_Core = NULL;
