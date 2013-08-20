/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Int.h"
#include "type/Value.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Helper for `intGet` and `intNth`, which acts like `bit` except that
 * out-of-range indices result in void, not the sign bit. `n` is always
 * passed as non-negative.
 */
static zvalue doIntNth(zvalue value, zint n) {
    if (n < collSize(value)) {
        zint intval = zintFromInt(value);
        zint result;
        zintBit(&result, intval, n); // Always succeeds
        return intFromZint(result);
    } else {
        return NULL;
    }
}


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intGet) {
    return doNthLenient(doIntNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intNth) {
    return doNthStrict(doIntNth, args[0], args[1]);
}
