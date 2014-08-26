// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bitwise.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(Bitwise) {
    MOD_USE(Value);

    SYM_INIT(and);
    SYM_INIT(bit);
    SYM_INIT(bitSize);
    SYM_INIT(not);
    SYM_INIT(or);
    SYM_INIT(shl);
    SYM_INIT(shr);
    SYM_INIT(xor);
}

// Documented in header.
SYM_DEF(and);

// Documented in header.
SYM_DEF(bit);

// Documented in header.
SYM_DEF(bitSize);

// Documented in header.
SYM_DEF(not);

// Documented in header.
SYM_DEF(or);

// Documented in header.
SYM_DEF(shl);

// Documented in header.
SYM_DEF(shr);

// Documented in header.
SYM_DEF(xor);
