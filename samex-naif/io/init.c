// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Initialization
//

#include "io.h"
#include "type/define.h"


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(io) {
    MOD_USE(Value);

    SYM_INIT(absent);
    SYM_INIT(directory);
    SYM_INIT(file);
    SYM_INIT(other);
    SYM_INIT(symlink);
}

// Documented in header.
SYM_DEF(absent);

// Documented in header.
SYM_DEF(directory);

// Documented in header.
SYM_DEF(file);

// Documented in header.
SYM_DEF(other);

// Documented in header.
SYM_DEF(symlink);
