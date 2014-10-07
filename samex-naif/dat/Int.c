// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>

#include "type/Core.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

enum {
    /** Count of cached small int value. */
    DAT_SMALL_INT_COUNT = DAT_SMALL_INT_MAX - DAT_SMALL_INT_MIN + 1
};

/** Array of small integer values. */
static zvalue SMALL_INTS[DAT_SMALL_INT_COUNT];

/**
 * Int structure.
 */
typedef struct {
    /** Int value. */
    zint value;
} IntInfo;

/**
 * Gets the value of the given int as a `zint`. Doesn't do any
 * type checking.
 */
static zint zintValue(zvalue intval) {
    return ((IntInfo *) datPayload(intval))->value;
}

/**
 * Constructs and returns an int.
 */
static zvalue intFrom(zint value) {
    zvalue result = datAllocValue(CLS_Int, sizeof(IntInfo));

    ((IntInfo *) datPayload(result))->value = value;
    return result;
}


//
// Exported Definitions
//

// Documented in header.
zvalue intFromZint(zint value) {
    if ((value >= DAT_SMALL_INT_MIN) && (value < DAT_SMALL_INT_MAX)) {
        return SMALL_INTS[value - DAT_SMALL_INT_MIN];
    } else {
        return intFrom(value);
    }
}

// Documented in header.
zint zintFromInt(zvalue intval) {
    assertHasClass(intval, CLS_Int);
    return zintValue(intval);
}


//
// Class Definition
//

// Documented in header.
zvalue INT_0 = NULL;

// Documented in header.
zvalue INT_1 = NULL;

// Documented in header.
zvalue INT_NEG1 = NULL;

/**
 * Helper for defining unary operations as methods.
 */
#define UNARY_IMPL(name, op) \
    METH_IMPL_0(Int, name) { \
        zint x = zintValue(ths); \
        zint result; \
        if ((op)(&result, x)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld).", x); \
        } \
    } \
    extern int semicolonRequiredHere

/**
 * Common helper for defining binary operations as methods. **Note:** `y` is
 * converted with `zintFromInt` because it does an explicit type check;
 * there's no guarantee that we're passed an `Int` per se.
 */
#define BINARY_IMPL(name, op) \
    METH_IMPL_1(Int, name, yVal) { \
        zint x = zintValue(ths); \
        zint y = zintFromInt(yVal); \
        zint result; \
        if ((op)(&result, x, y)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld, %lld).", x, y); \
        } \
    } \
    extern int semicolonRequiredHere

// All documented in header.
UNARY_IMPL(abs,     zintAbs);
UNARY_IMPL(bitSize, zintSafeBitSize);
UNARY_IMPL(neg,     zintNeg);
UNARY_IMPL(not,     zintNot);
UNARY_IMPL(sign,    zintSign);

// All documented in header.
BINARY_IMPL(add,   zintAdd);
BINARY_IMPL(and,   zintAnd);
BINARY_IMPL(bit,   zintBit);
BINARY_IMPL(div,   zintDiv);
BINARY_IMPL(divEu, zintDivEu);
BINARY_IMPL(mod,   zintMod);
BINARY_IMPL(modEu, zintModEu);
BINARY_IMPL(mul,   zintMul);
BINARY_IMPL(or,    zintOr);
BINARY_IMPL(shl,   zintShl);
BINARY_IMPL(shr,   zintShr);
BINARY_IMPL(sub,   zintSub);
BINARY_IMPL(xor,   zintXor);

// Documented in header.
METH_IMPL_0(Int, debugString) {
    char arr[22];  // Big enough for the longest possible result.

    snprintf(arr, sizeof(arr), "%lld", zintValue(ths));
    return stringFromUtf8(-1, arr);
}

// Documented in header.
METH_IMPL_0(Int, toInt) {
    return ths;
}

// Documented in header.
METH_IMPL_0(Int, toNumber) {
    return ths;
}

// Documented in header.
METH_IMPL_0(Int, toString) {
    zint n = zintValue(ths);
    zchar result;

    if (!zcharFromZint(&result, n)) {
        die("Invalid int value for char: %lld", n);
    }

    return stringFromZchar(result);
}

// Documented in header.
METH_IMPL_1(Int, totalEq, other) {
    // Note: `other` not guaranteed to be an `Int`.
    return (zintValue(ths) == zintFromInt(other)) ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(Int, totalOrder, other) {
    zint int1 = zintValue(ths);
    zint int2 = zintFromInt(other);  // Note: not guaranteed to be an `Int`.

    if (int1 < int2) {
        return INT_NEG1;
    } else if (int1 > int2) {
        return INT_1;
    } else {
        return INT_0;
    }
}

/** Initializes the module. */
MOD_INIT(Int) {
    MOD_USE(OneOff);

    CLS_Int = makeCoreClass("Int", CLS_Core,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Int, abs),
            METH_BIND(Int, add),
            METH_BIND(Int, and),
            METH_BIND(Int, bit),
            METH_BIND(Int, bitSize),
            METH_BIND(Int, debugString),
            METH_BIND(Int, div),
            METH_BIND(Int, divEu),
            METH_BIND(Int, mod),
            METH_BIND(Int, modEu),
            METH_BIND(Int, mul),
            METH_BIND(Int, neg),
            METH_BIND(Int, not),
            METH_BIND(Int, or),
            METH_BIND(Int, shl),
            METH_BIND(Int, shr),
            METH_BIND(Int, sign),
            METH_BIND(Int, sub),
            METH_BIND(Int, xor),
            METH_BIND(Int, toInt),
            METH_BIND(Int, toNumber),
            METH_BIND(Int, toString),
            METH_BIND(Int, totalEq),
            METH_BIND(Int, totalOrder),
            NULL));

    for (zint i = 0; i < DAT_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = datImmortalize(intFrom(i + DAT_SMALL_INT_MIN));
    }

    INT_0    = intFromZint(0);
    INT_1    = intFromZint(1);
    INT_NEG1 = intFromZint(-1);
}

// Documented in header.
zvalue CLS_Int = NULL;
