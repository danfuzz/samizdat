// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bitwise.h"
#include "type/Bool.h"
#include "type/Data.h"
#include "type/Int.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Bool structure.
 */
typedef struct {
    /** Boolean value. */
    zbool value;
} BoolInfo;

/**
 * Gets the value of the given bool as a `zbool`. Doesn't do any
 * type checking.
 */
static zbool zboolValue(zvalue boolval) {
    return ((BoolInfo *) datPayload(boolval))->value;
}

/**
 * Constructs and returns a bool.
 */
static zvalue boolFrom(zbool value) {
    zvalue result = datAllocValue(CLS_Bool, sizeof(BoolInfo));

    ((BoolInfo *) datPayload(result))->value = value;
    return result;
}


//
// Exported Definitions
//

// Documented in header.
zvalue boolFromZbool(zbool value) {
    return value ? BOOL_TRUE : BOOL_FALSE;
}

// Documented in header.
zbool zboolFromBool(zvalue boolval) {
    assertHasClass(boolval, CLS_Bool);
    return zboolValue(boolval);
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Bool, and) {
    zbool bool1 = zboolValue(args[0]);
    zbool bool2 = zboolFromBool(args[1]);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 & bool2);
}

// Documented in header.
METH_IMPL(Bool, bit) {
    zvalue boolval = args[0];
    zint n = zintFromInt(args[1]);

    if (n == 0) {
        return intFromZint(zboolValue(boolval));
    } else if (n > 0) {
        return INT_0;
    } else {
        return NULL;
    }
}

// Documented in header.
METH_IMPL(Bool, bitSize) {
    return INT_1;
}

// Documented in header.
METH_IMPL(Bool, or) {
    zbool bool1 = zboolValue(args[0]);
    zbool bool2 = zboolFromBool(args[1]);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 | bool2);
}

// Documented in header.
METH_IMPL(Bool, not) {
    zvalue boolval = args[0];
    return boolFromZbool(!zboolValue(boolval));
}

// Documented in header.
METH_IMPL(Bool, shl) {
    zvalue value = args[0];
    zvalue n = args[1];
    zint amt = zintFromInt(n);  // Includes a type check.

    if (!zboolValue(value)) {
        // `false` can be shifted howsoever and still be false.
        return value;
    }

    if (amt == 0) {
        return value;
    } else if (amt < 0) {
        return BOOL_FALSE;
    } else {
        die("Undefined `bool` shift result.");
    }
}

// Documented in header.
METH_IMPL(Bool, shr) {
    zvalue value = args[0];
    zvalue n = args[1];
    zint amt = zintFromInt(n);  // Includes a type check.

    if (!zboolValue(value)) {
        // `false` can be shifted howsoever and still be false.
        return value;
    }

    if (amt == 0) {
        return value;
    } else if (amt > 0) {
        return BOOL_FALSE;
    } else {
        die("Undefined `bool` shift result.");
    }
}

// Documented in header.
METH_IMPL(Bool, toInt) {
    zvalue boolval = args[0];
    return intFromZint(zboolValue(boolval));
}

// Documented in header.
METH_IMPL(Bool, toNumber) {
    zvalue boolval = args[0];
    return intFromZint(zboolValue(boolval));
}

// Documented in header.
METH_IMPL(Bool, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `Bool`.
    return (zboolValue(value) == zboolFromBool(other)) ? value : NULL;
}

// Documented in header.
METH_IMPL(Bool, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `Bool`.
    bool bool1 = zboolValue(value);
    bool bool2 = zboolFromBool(other);

    if (bool1 == bool2) {
        return INT_0;
    } else if (bool1) {
        return INT_1;
    } else {
        return INT_NEG1;
    }
}

// Documented in header.
METH_IMPL(Bool, xor) {
    zbool bool1 = zboolValue(args[0]);
    zbool bool2 = zboolFromBool(args[1]);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 ^ bool2);
}

/** Initializes the module. */
MOD_INIT(Bool) {
    MOD_USE(Bitwise);
    MOD_USE(Int);
    MOD_USE(OneOff);

    CLS_Bool = makeCoreClass("Bool", CLS_Data,
        NULL,
        selectorTableFromArgs(
            SEL_METH(Bool, and),
            SEL_METH(Bool, bit),
            SEL_METH(Bool, bitSize),
            SEL_METH(Bool, not),
            SEL_METH(Bool, or),
            SEL_METH(Bool, shl),
            SEL_METH(Bool, shr),
            SEL_METH(Bool, xor),
            SEL_METH(Bool, toInt),
            SEL_METH(Bool, toNumber),
            SEL_METH(Bool, totalEq),
            SEL_METH(Bool, totalOrder),
            NULL));

    BOOL_FALSE = boolFrom(false);
    datImmortalize(BOOL_FALSE);

    BOOL_TRUE = boolFrom(true);
    datImmortalize(BOOL_TRUE);
}

// Documented in header.
zvalue CLS_Bool = NULL;

// Documented in header.
zvalue BOOL_FALSE = NULL;

// Documented in header.
zvalue BOOL_TRUE = NULL;
