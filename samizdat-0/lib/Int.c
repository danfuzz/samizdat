/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/Int.h"
#include "type/Value.h"
#include "util.h"


/*
 * Private Definitions
 */

#define UNARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        zint x = zintFromInt(args[0]); \
        zint result; \
        if ((op)(&result, x)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld).", x); \
        } \
    } \
    extern int semicolonRequiredHere

#define BINARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        zint x = zintFromInt(args[0]); \
        zint y = zintFromInt(args[1]); \
        zint result; \
        if ((op)(&result, x, y)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld, %lld).", x, y); \
        } \
    } \
    extern int semicolonRequiredHere

/**
 * Helper for `intGet` and `intNth`, which acts like `ibit` except that
 * out-of-range indices result in void, not the sign bit. `n` is always
 * passed as non-negative.
 */
static zvalue doIntNth(zvalue value, zint n) {
    if (n < valSize(value)) {
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

/* These are all documented in Samizdat Layer 0 spec. */
BINARY_PRIM(ibit,   zintBit);
BINARY_PRIM(imul,   zintMul);
BINARY_PRIM(ior,    zintOr);
BINARY_PRIM(ishl,   zintShl);
BINARY_PRIM(ishr,   zintShr);
BINARY_PRIM(isub,   zintSub);
BINARY_PRIM(ixor,   zintXor);

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intGet) {
    return doNthLenient(doIntNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intNth) {
    return doNthStrict(doIntNth, args[0], args[1]);
}
