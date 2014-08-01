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

    SEL_INIT(2, 2, and);

    SEL_INIT(2, 2, bit);

    SEL_INIT(1, 1, bitSize);

    SEL_INIT(1, 1, not);

    SEL_INIT(2, 2, or);

    SEL_INIT(2, 2, shl);

    SEL_INIT(2, 2, xor);
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
