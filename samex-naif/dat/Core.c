// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

// Documented in header.
void bindMethodsForCore(void) {
    classBindMethods(CLS_Core,
        NULL,
        NULL);
}

/** Initializes the module. */
MOD_INIT(Core) {
    MOD_USE(Value);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForValue()`.
}

// Documented in header.
zvalue CLS_Core = NULL;
