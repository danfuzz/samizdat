/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

enum {
    /** Count of cached small int value. */
    PB_SMALL_INT_COUNT = PB_SMALL_INT_MAX - PB_SMALL_INT_MIN + 1
};

/** Array of small integer values. */
static zvalue SMALL_INTS[PB_SMALL_INT_COUNT];

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
    return ((IntInfo *) pbPayload(intval))->value;
}

/**
 * Constructs and returns an int.
 */
zvalue intFrom(zint value) {
    zint size = zintBitSize(value);
    zvalue result = pbAllocValue(TYPE_Int, sizeof(IntInfo));

    ((IntInfo *) pbPayload(result))->value = value;
    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zchar zcharFromInt(zvalue intval) {
    zint n = zintFromInt(intval);
    zchar result;

    if (!zcharFromZint(&result, n)) {
        die("Invalid int value for zchar: %lld", n);
    }

    return result;
}

/* Documented in header. */
zvalue intFromZint(zint value) {
    if ((value >= PB_SMALL_INT_MIN) && (value < PB_SMALL_INT_MAX)) {
        return SMALL_INTS[value - PB_SMALL_INT_MIN];
    } else {
        return intFrom(value);
    }
}

/* Documented in header. */
zint zintFromInt(zvalue intval) {
    assertHasType(intval, TYPE_Int);
    return zintValue(intval);
}


/*
 * Type Definition
 */

/* Documented in header. */
zvalue PB_0 = NULL;

/* Documented in header. */
zvalue PB_1 = NULL;

/* Documented in header. */
zvalue PB_NEG1 = NULL;

/**
 * Helper for defining unary operations as methods.
 */
#define UNARY_IMPL(name, op) \
    METH_IMPL(Int, name) { \
        zint x = zintFromInt(args[0]); \
        zint result; \
        if ((op)(&result, x)) { \
            return intFromZint(result); \
        } else { \
            die("Overflow / error on" #name "(%lld).", x); \
        } \
    } \
    extern int semicolonRequiredHere

// All documented in header.
UNARY_IMPL(abs,  zintAbs);
UNARY_IMPL(neg,  zintNeg);
UNARY_IMPL(not,  zintNot);
UNARY_IMPL(sign, zintSign);

/* Documented in header. */
METH_IMPL(Int, eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (zintValue(v1) == zintValue(v2)) ? v2 : NULL;
}

/* Documented in header. */
METH_IMPL(Int, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zint int1 = zintValue(v1);
    zint int2 = zintValue(v2);

    if (int1 < int2) {
        return PB_NEG1;
    } else if (int1 > int2) {
        return PB_1;
    } else {
        return PB_0;
    }
}

/* Documented in header. */
METH_IMPL(Int, size) {
    zvalue intval = args[0];

    return intFromZint(zintBitSize(zintValue(intval)));
}

/* Documented in header. */
void pbBindInt(void) {
    GFN_abs = makeGeneric(1, 1, stringFromUtf8(-1, "abs"));
    pbImmortalize(GFN_abs);

    GFN_neg = makeGeneric(1, 1, stringFromUtf8(-1, "neg"));
    pbImmortalize(GFN_neg);

    GFN_not = makeGeneric(1, 1, stringFromUtf8(-1, "not"));
    pbImmortalize(GFN_not);

    GFN_sign = makeGeneric(1, 1, stringFromUtf8(-1, "sign"));
    pbImmortalize(GFN_sign);

    TYPE_Int = coreTypeFromName(stringFromUtf8(-1, "Int"), false);
    METH_BIND(Int, abs);
    METH_BIND(Int, eq);
    METH_BIND(Int, order);
    METH_BIND(Int, neg);
    METH_BIND(Int, not);
    METH_BIND(Int, sign);
    METH_BIND(Int, size);

    for (zint i = 0; i < PB_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = intFrom(i + PB_SMALL_INT_MIN);
        pbImmortalize(SMALL_INTS[i]);
    }

    PB_0    = intFromZint(0);
    PB_1    = intFromZint(1);
    PB_NEG1 = intFromZint(-1);
}

/* Documented in header. */
zvalue TYPE_Int = NULL;

/* Documented in header. */
zvalue GFN_abs = NULL;

/** Generic `neg(int)`: Documented in spec. */
zvalue GFN_neg = NULL;

/** Generic `not(int)`: Documented in spec. */
zvalue GFN_not = NULL;

/** Generic `sign(int)`: Documented in spec. */
zvalue GFN_sign = NULL;
