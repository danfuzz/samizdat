// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Number.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(Number) {
    MOD_USE(Value);

    SEL_INIT(1, 1, abs);

    SEL_INIT(2, 2, add);

    SEL_INIT(2, 2, div);

    SEL_INIT(2, 2, divEu);

    SEL_INIT(2, 2, mod);

    SEL_INIT(2, 2, modEu);

    SEL_INIT(2, 2, mul);

    SEL_INIT(1, 1, neg);

    SEL_INIT(1, 1, sign);

    SEL_INIT(2, 2, sub);
}

// Documented in header.
SEL_DEF(abs);

// Documented in header.
SEL_DEF(add);

// Documented in header.
SEL_DEF(div);

// Documented in header.
SEL_DEF(divEu);

// Documented in header.
SEL_DEF(mod);

// Documented in header.
SEL_DEF(modEu);

// Documented in header.
SEL_DEF(mul);

// Documented in header.
SEL_DEF(neg);

// Documented in header.
SEL_DEF(sign);

// Documented in header.
SEL_DEF(sub);
