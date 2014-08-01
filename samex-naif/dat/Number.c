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

    SEL_INIT(abs);
    SEL_INIT(add);
    SEL_INIT(div);
    SEL_INIT(divEu);
    SEL_INIT(mod);
    SEL_INIT(modEu);
    SEL_INIT(mul);
    SEL_INIT(neg);
    SEL_INIT(sign);
    SEL_INIT(sub);
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
