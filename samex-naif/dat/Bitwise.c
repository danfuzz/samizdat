/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "type/Bitwise.h"
#include "type/Generic.h"
#include "type/String.h"

#include "impl.h"


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(Bitwise) {
    MOD_USE(Value);

    GFN_and = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "and"));
    datImmortalize(GFN_and);

    GFN_bit = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "bit"));
    datImmortalize(GFN_bit);

    GFN_bitSize = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "bitSize"));
    datImmortalize(GFN_bitSize);

    GFN_not = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "not"));
    datImmortalize(GFN_not);

    GFN_or = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "or"));
    datImmortalize(GFN_or);

    GFN_shl = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "shl"));
    datImmortalize(GFN_shl);

    GFN_xor = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "xor"));
    datImmortalize(GFN_xor);
}

/* Documented in header. */
zvalue GFN_and = NULL;

/* Documented in header. */
zvalue GFN_bit = NULL;

/* Documented in header. */
zvalue GFN_bitSize = NULL;

/* Documented in header. */
zvalue GFN_not = NULL;

/* Documented in header. */
zvalue GFN_or = NULL;

/* Documented in header. */
zvalue GFN_shl = NULL;

/* Documented in header. */
zvalue GFN_xor = NULL;
