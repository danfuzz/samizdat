/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Helper definitions
 */

#define UNARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        requireExactly(argCount, 1); \
        zint x = datIntFromIntlet(args[0]); \
        return datIntletFromInt((op)); \
    } \
    extern int semicolonRequiredHere

#define BINARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        requireExactly(argCount, 2); \
        zint x = datIntFromIntlet(args[0]); \
        zint y = datIntFromIntlet(args[1]); \
        return datIntletFromInt((op)); \
    } \
    extern int semicolonRequiredHere

/**
 * Binary left-shift operation. This isn't a straightforward
 * application of `<<` to the two arguments, because:
 *
 * * The C spec demands that the right-hand argument be truncated to
 *   type `int`.
 *
 * * The C spec allows unpredictable behavior when the right-hand argument
 *   is negative. In *Samizdat Layer 0* negative shifts indicate a
 *   right-shift operation.
 *
 * * We need to preserve the sign of the result, since we are simulating
 *   infinite-width integers.
 *
 * *Note:* We limit shifts to 32 bits here, relying on this to be
 * meaning-preserving in the face of the `dat` layer's implementation
 * of the 32-bit width restriction on intlets.
 */
static zint shiftLeft(zint value, zint shift) {
    if (shift == 0) {
        return value;
    } else if (shift > 0) {
        // A left shift.
        if (shift > 32) {
            shift = 32;
        }
        return value << shift;
    } else {
        // A right shift.
        shift = -shift;
        if (shift > 32) {
            shift = 32;
        }
        return value >> shift;
    }
}


/*
 * Exported primitives. These are all documented in Samizdat Layer 0
 * spec.
 */

UNARY_PRIM(ineg, -x);
UNARY_PRIM(inot, ~x);

BINARY_PRIM(iadd, x + y);
BINARY_PRIM(iand, x & y);
BINARY_PRIM(idiv, x / y);
BINARY_PRIM(imod, x % y);
BINARY_PRIM(imul, x * y);
BINARY_PRIM(ior,  x | y);
BINARY_PRIM(isub, x - y);
BINARY_PRIM(ishl, shiftLeft(x, y));
BINARY_PRIM(ishr, shiftLeft(x, -y));
BINARY_PRIM(ixor, x ^ y);
