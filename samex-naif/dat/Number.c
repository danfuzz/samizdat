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

    SYM_INIT(abs);
    SYM_INIT(add);
    SYM_INIT(div);
    SYM_INIT(divEu);
    SYM_INIT(mod);
    SYM_INIT(modEu);
    SYM_INIT(mul);
    SYM_INIT(neg);
    SYM_INIT(sign);
    SYM_INIT(sub);
}

// Documented in header.
SYM_DEF(abs);

// Documented in header.
SYM_DEF(add);

// Documented in header.
SYM_DEF(div);

// Documented in header.
SYM_DEF(divEu);

// Documented in header.
SYM_DEF(mod);

// Documented in header.
SYM_DEF(modEu);

// Documented in header.
SYM_DEF(mul);

// Documented in header.
SYM_DEF(neg);

// Documented in header.
SYM_DEF(sign);

// Documented in header.
SYM_DEF(sub);
