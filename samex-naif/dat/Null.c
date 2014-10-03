// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Int.h"
#include "type/Null.h"
#include "type/String.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

// Documented in header.
METH_IMPL_0(Null, debugString) {
    return stringFromUtf8(-1, "null");
}

// Documented in header.
METH_IMPL_1(Null, totalEq, other) {
    // Note: `other` not guaranteed to be `null`.
    if (ths == other) {
        return ths;
    } else {
        die("`totalEq` called with incompatible arguments.");
    }
}

// Documented in header.
METH_IMPL_1(Null, totalOrder, other) {
    // Note: `other` not guaranteed to be `null`.
    if (ths == other) {
        return INT_0;
    } else {
        die("`totalOrder` called with incompatible arguments.");
    }
}

/** Initializes the module. */
MOD_INIT(Null) {
    MOD_USE(Int);

    CLS_Null = makeCoreClass("Null", CLS_Core,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Null, debugString),
            METH_BIND(Null, totalEq),
            METH_BIND(Null, totalOrder),
            NULL));

    THE_NULL = datAllocValue(CLS_Null, 0);
    datImmortalize(THE_NULL);
}

// Documented in header.
zvalue CLS_Null = NULL;

// Documented in header.
zvalue THE_NULL = NULL;
