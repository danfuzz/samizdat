// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bitwise.h"
#include "type/Data.h"
#include "type/Int.h"
#include "type/Number.h"
#include "type/define.h"
#include "zlimits.h"

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
    METH_IMPL(Int, name) { \
        zint x = zintValue(args[0]); \
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
    METH_IMPL(Int, name) { \
        zint x = zintValue(args[0]); \
        zint y = zintFromInt(args[1]); \
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
METH_IMPL(Int, toInt) {
    zvalue intval = args[0];
    return intval;
}

// Documented in header.
METH_IMPL(Int, toNumber) {
    zvalue intval = args[0];
    return intval;
}

// Documented in header.
METH_IMPL(Int, toString) {
    zvalue intval = args[0];

    zint n = zintFromInt(intval);
    zchar result;

    if (!zcharFromZint(&result, n)) {
        die("Invalid int value for char: %lld", n);
    }

    return stringFromZchar(result);
}

// Documented in header.
METH_IMPL(Int, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be an `Int`.
    return (zintValue(value) == zintFromInt(other)) ? value : NULL;
}

// Documented in header.
METH_IMPL(Int, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be an `Int`.
    zint int1 = zintValue(value);
    zint int2 = zintFromInt(other);

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
    MOD_USE(Bitwise);
    MOD_USE(Number);
    MOD_USE(OneOff);

    CLS_Int = makeCoreClass("Int", CLS_Data,
        NULL,
        selectorTableFromArgs(
            SEL_METH(Int, abs),
            SEL_METH(Int, add),
            SEL_METH(Int, and),
            SEL_METH(Int, bit),
            SEL_METH(Int, bitSize),
            SEL_METH(Int, div),
            SEL_METH(Int, divEu),
            SEL_METH(Int, mod),
            SEL_METH(Int, modEu),
            SEL_METH(Int, mul),
            SEL_METH(Int, neg),
            SEL_METH(Int, not),
            SEL_METH(Int, or),
            SEL_METH(Int, shl),
            SEL_METH(Int, shr),
            SEL_METH(Int, sign),
            SEL_METH(Int, sub),
            SEL_METH(Int, xor),
            SEL_METH(Int, toInt),
            SEL_METH(Int, toNumber),
            SEL_METH(Int, toString),
            SEL_METH(Int, totalEq),
            SEL_METH(Int, totalOrder),
            NULL));

    for (zint i = 0; i < DAT_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = intFrom(i + DAT_SMALL_INT_MIN);
        datImmortalize(SMALL_INTS[i]);
    }

    INT_0    = intFromZint(0);
    INT_1    = intFromZint(1);
    INT_NEG1 = intFromZint(-1);
}

// Documented in header.
zvalue CLS_Int = NULL;
