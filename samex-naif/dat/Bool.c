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
METH_IMPL_1(Bool, and, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 & bool2);
}

// Documented in header.
METH_IMPL_1(Bool, bit, n) {
    zint ni = zintFromInt(n);

    if (ni == 0) {
        return intFromZint(zboolValue(ths));
    } else if (ni > 0) {
        return INT_0;
    } else {
        return NULL;
    }
}

// Documented in header.
METH_IMPL_0(Bool, bitSize) {
    return INT_1;
}

// Documented in header.
METH_IMPL_1(Bool, or, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 | bool2);
}

// Documented in header.
METH_IMPL_0(Bool, not) {
    return boolFromZbool(!zboolValue(ths));
}

// Documented in header.
METH_IMPL_1(Bool, shl, n) {
    zint ni = zintFromInt(n);

    if (!zboolValue(ths)) {
        // `false` can be shifted howsoever and still be false.
        return ths;
    }

    if (ni == 0) {
        return ths;
    } else if (ni < 0) {
        return BOOL_FALSE;
    } else {
        die("Undefined `bool` shift result.");
    }
}

// Documented in header.
METH_IMPL_1(Bool, shr, n) {
    zint ni = zintFromInt(n);

    if (!zboolValue(ths)) {
        // `false` can be shifted howsoever and still be false.
        return ths;
    }

    if (ni == 0) {
        return ths;
    } else if (ni > 0) {
        return BOOL_FALSE;
    } else {
        die("Undefined `bool` shift result.");
    }
}

// Documented in header.
METH_IMPL_0(Bool, toInt) {
    return intFromZint(zboolValue(ths));
}

// Documented in header.
METH_IMPL_0(Bool, toNumber) {
    return intFromZint(zboolValue(ths));
}

// Documented in header.
METH_IMPL_1(Bool, totalEq, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return (bool1 == bool2) ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(Bool, totalOrder, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.

    if (bool1 == bool2) {
        return INT_0;
    } else if (bool1) {
        return INT_1;
    } else {
        return INT_NEG1;
    }
}

// Documented in header.
METH_IMPL_1(Bool, xor, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 ^ bool2);
}

/** Initializes the module. */
MOD_INIT(Bool) {
    MOD_USE(Bitwise);
    MOD_USE(Int);
    MOD_USE(OneOff);

    CLS_Bool = makeCoreClass("Bool", CLS_Data,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Bool, and),
            METH_BIND(Bool, bit),
            METH_BIND(Bool, bitSize),
            METH_BIND(Bool, not),
            METH_BIND(Bool, or),
            METH_BIND(Bool, shl),
            METH_BIND(Bool, shr),
            METH_BIND(Bool, xor),
            METH_BIND(Bool, toInt),
            METH_BIND(Bool, toNumber),
            METH_BIND(Bool, totalEq),
            METH_BIND(Bool, totalOrder),
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
