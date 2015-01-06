// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Initialization
//

#include "module.h"


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(lang) {
    MOD_USE(cls);
    MOD_USE(langnode);
    MOD_USE(Closure);
    MOD_USE(ExecNode);
}
