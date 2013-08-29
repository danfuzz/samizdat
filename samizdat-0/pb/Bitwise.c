/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Bitwise.h"
#include "type/Generic.h"
#include "type/String.h"


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindBitwise(void) {
    GFN_and = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "and"));
    pbImmortalize(GFN_and);

    GFN_bit = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "bit"));
    pbImmortalize(GFN_bit);

    GFN_bitSize = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "bitSize"));
    pbImmortalize(GFN_bitSize);

    GFN_not = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "not"));
    pbImmortalize(GFN_not);

    GFN_or = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "or"));
    pbImmortalize(GFN_or);

    GFN_shl = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "shl"));
    pbImmortalize(GFN_shl);

    GFN_xor = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "xor"));
    pbImmortalize(GFN_xor);
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
