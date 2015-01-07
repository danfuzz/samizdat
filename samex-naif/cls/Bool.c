// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bool.h"
#include "type/Cmp.h"
#include "type/Core.h"
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

// Documented in spec.
CMETH_IMPL_1(Bool, castFrom, value) {
    zvalue cls = classOf(value);

    if (cmpEq(cls, CLS_Int)) {
        zint n = zintFromInt(value);
        switch (n) {
            case 0: return BOOL_FALSE;
            case 1: return BOOL_TRUE;
        }
    } else if (typeAccepts(thsClass, value)) {
        return value;
    }

    return NULL;
}

// Documented in spec.
CMETH_IMPL_0_opt(Bool, fromLogic, value) {
    return (value == NULL) ? BOOL_FALSE : BOOL_TRUE;
}

// Documented in spec.
METH_IMPL_1(Bool, and, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 & bool2);
}

// Documented in spec.
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

// Documented in spec.
METH_IMPL_0(Bool, bitSize) {
    return INT_1;
}

// Documented in spec.
METH_IMPL_1(Bool, castToward, cls) {
    if (cmpEq(cls, CLS_Int)) {
        return intFromZint(zboolValue(ths));
    } else if (typeAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_1(Bool, crossEq, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return (bool1 == bool2) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Bool, crossOrder, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.

    if (bool1 == bool2) {
        return SYM(same);
    } else if (bool1) {
        return SYM(more);
    } else {
        return SYM(less);
    }
}

// Documented in spec.
METH_IMPL_0(Bool, debugString) {
    return stringFromUtf8(-1, zboolValue(ths) ? "true" : "false");
}

// Documented in spec.
METH_IMPL_1(Bool, or, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 | bool2);
}

// Documented in spec.
METH_IMPL_0(Bool, not) {
    return boolFromZbool(!zboolValue(ths));
}

// Documented in spec.
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

// Documented in spec.
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

// Documented in spec.
METH_IMPL_0(Bool, toLogic) {
    return zboolValue(ths) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Bool, xor, other) {
    zbool bool1 = zboolValue(ths);
    zbool bool2 = zboolFromBool(other);  // Not guaranteed to be a `Bool`.
    return boolFromZbool(bool1 ^ bool2);
}

/** Initializes the module. */
MOD_INIT(Bool) {
    MOD_USE(Value);

    CLS_Bool = makeCoreClass(SYM(Bool), CLS_Core,
        METH_TABLE(
            CMETH_BIND(Bool, castFrom),
            CMETH_BIND(Bool, fromLogic)),
        METH_TABLE(
            METH_BIND(Bool, and),
            METH_BIND(Bool, bit),
            METH_BIND(Bool, bitSize),
            METH_BIND(Bool, castToward),
            METH_BIND(Bool, crossEq),
            METH_BIND(Bool, crossOrder),
            METH_BIND(Bool, debugString),
            METH_BIND(Bool, not),
            METH_BIND(Bool, or),
            METH_BIND(Bool, shl),
            METH_BIND(Bool, shr),
            METH_BIND(Bool, xor),
            METH_BIND(Bool, toLogic)));

    BOOL_FALSE = datImmortalize(boolFrom(false));
    BOOL_TRUE = datImmortalize(boolFrom(true));
}

// Documented in header.
zvalue CLS_Bool = NULL;

// Documented in header.
zvalue BOOL_FALSE = NULL;

// Documented in header.
zvalue BOOL_TRUE = NULL;
