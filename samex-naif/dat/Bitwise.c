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

    SEL_INIT(and);
    SEL_INIT(bit);
    SEL_INIT(bitSize);
    SEL_INIT(not);
    SEL_INIT(or);
    SEL_INIT(shl);
    SEL_INIT(xor);
}

// Documented in header.
SEL_DEF(and);

// Documented in header.
SEL_DEF(bit);

// Documented in header.
SEL_DEF(bitSize);

// Documented in header.
SEL_DEF(not);

// Documented in header.
SEL_DEF(or);

// Documented in header.
SEL_DEF(shl);

// Documented in header.
SEL_DEF(xor);
