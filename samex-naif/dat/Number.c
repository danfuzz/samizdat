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

    GFN_abs = makeGeneric(1, 1, stringFromUtf8(-1, "abs"));
    datImmortalize(GFN_abs);

    GFN_add = makeGeneric(2, 2, stringFromUtf8(-1, "add"));
    datImmortalize(GFN_add);

    GFN_div = makeGeneric(2, 2, stringFromUtf8(-1, "div"));
    datImmortalize(GFN_div);

    GFN_divEu = makeGeneric(2, 2, stringFromUtf8(-1, "divEu"));
    datImmortalize(GFN_divEu);

    GFN_mod = makeGeneric(2, 2, stringFromUtf8(-1, "mod"));
    datImmortalize(GFN_mod);

    GFN_modEu = makeGeneric(2, 2, stringFromUtf8(-1, "modEu"));
    datImmortalize(GFN_modEu);

    GFN_mul = makeGeneric(2, 2, stringFromUtf8(-1, "mul"));
    datImmortalize(GFN_mul);

    GFN_neg = makeGeneric(1, 1, stringFromUtf8(-1, "neg"));
    datImmortalize(GFN_neg);

    GFN_sign = makeGeneric(1, 1, stringFromUtf8(-1, "sign"));
    datImmortalize(GFN_sign);

    GFN_sub = makeGeneric(2, 2, stringFromUtf8(-1, "sub"));
    datImmortalize(GFN_sub);
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
