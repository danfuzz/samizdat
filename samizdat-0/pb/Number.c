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

/* Documented in header. */
void pbBindNumber(void) {
    GFN_abs = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "abs"));
    pbImmortalize(GFN_abs);

    GFN_add = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "add"));
    pbImmortalize(GFN_add);

    GFN_div = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "div"));
    pbImmortalize(GFN_div);

    GFN_divEu = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "divEu"));
    pbImmortalize(GFN_divEu);

    GFN_mod = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "mod"));
    pbImmortalize(GFN_mod);

    GFN_modEu = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "modEu"));
    pbImmortalize(GFN_modEu);

    GFN_mul = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "mul"));
    pbImmortalize(GFN_mul);

    GFN_neg = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "neg"));
    pbImmortalize(GFN_neg);

    GFN_sign = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "sign"));
    pbImmortalize(GFN_sign);

    GFN_sub = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "sub"));
    pbImmortalize(GFN_sub);
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
