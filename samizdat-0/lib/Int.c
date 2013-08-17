/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"


/*
 * Private Definitions
 */

#define UNARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        zint x = zintFromInt(args[0]); \
        return intFromZint((op)); \
    } \
    extern int semicolonRequiredHere

#define BINARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        zint x = zintFromInt(args[0]); \
        zint y = zintFromInt(args[1]); \
        return intFromZint((op)); \
    } \
    extern int semicolonRequiredHere

#define ZBINARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        zint x = zintFromInt(args[0]); \
        zint y = zintFromInt(args[1]); \
        zint result; \
        if ((op)(&result, x, y)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld, %lld).", x, y); \
        } \
    }

/**
 * Helper for `intGet` and `intNth`, which acts like `ibit` except that
 * out-of-range indices result in void, not the sign bit. `n` is always
 * passed as non-negative.
 */
static zvalue doIntNth(zvalue value, zint n) {
    if (n < pbSize(value)) {
        zint intval = zintFromInt(value);
        zint result;
        zintGetBit(&result, intval, n); // Always succeeds
        return intFromZint(result);
    } else {
        return NULL;
    }
}


/*
 * Exported Definitions
 */

/* These are all documented in Samizdat Layer 0 spec. */
UNARY_PRIM(iabs,  (x >= 0) ? x : -x);
UNARY_PRIM(ineg,  -x);
UNARY_PRIM(inot,  ~x);
UNARY_PRIM(isign, (x == 0) ? 0 : ((x > 0) ? 1 : -1));

/* These are all documented in Samizdat Layer 0 spec. */

BINARY_PRIM(iand, x & y);
BINARY_PRIM(ior,  x | y);
BINARY_PRIM(ixor, x ^ y);

ZBINARY_PRIM(iadd,   zintAdd);
ZBINARY_PRIM(idiv,   zintDiv);
ZBINARY_PRIM(idivEu, zintDivEu);
ZBINARY_PRIM(ibit,   zintGetBit);
ZBINARY_PRIM(imod,   zintMod);
ZBINARY_PRIM(imodEu, zintModEu);
ZBINARY_PRIM(imul,   zintMul);
ZBINARY_PRIM(ishl,   zintShl);
ZBINARY_PRIM(ishr,   zintShr);
ZBINARY_PRIM(isub,   zintSub);

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intGet) {
    return doNthLenient(doIntNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intNth) {
    return doNthStrict(doIntNth, args[0], args[1]);
}
