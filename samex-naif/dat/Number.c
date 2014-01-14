/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Number.h"
#include "type/Generic.h"
#include "type/String.h"


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(Number) {
    MOD_USE(Value);

    GFN_abs = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "abs"));
    datImmortalize(GFN_abs);

    GFN_add = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "add"));
    datImmortalize(GFN_add);

    GFN_div = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "div"));
    datImmortalize(GFN_div);

    GFN_divEu = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "divEu"));
    datImmortalize(GFN_divEu);

    GFN_mod = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "mod"));
    datImmortalize(GFN_mod);

    GFN_modEu = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "modEu"));
    datImmortalize(GFN_modEu);

    GFN_mul = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "mul"));
    datImmortalize(GFN_mul);

    GFN_neg = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "neg"));
    datImmortalize(GFN_neg);

    GFN_sign = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "sign"));
    datImmortalize(GFN_sign);

    GFN_sub = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "sub"));
    datImmortalize(GFN_sub);
}

/* Documented in header. */
zvalue GFN_abs = NULL;

/* Documented in header. */
zvalue GFN_add = NULL;

/* Documented in header. */
zvalue GFN_div = NULL;

/* Documented in header. */
zvalue GFN_divEu = NULL;

/* Documented in header. */
zvalue GFN_mod = NULL;

/* Documented in header. */
zvalue GFN_modEu = NULL;

/* Documented in header. */
zvalue GFN_mul = NULL;

/* Documented in header. */
zvalue GFN_neg = NULL;

/* Documented in header. */
zvalue GFN_sign = NULL;

/* Documented in header. */
zvalue GFN_sub = NULL;
