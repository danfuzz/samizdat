// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Int.h"
#include "type/Null.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

// Documented in header.
METH_IMPL(Null, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be `null`.

    if (value == other) {
        return value;
    } else {
        die("`totalEq` called with incompatible arguments.");
    }
}

// Documented in header.
METH_IMPL(Null, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be `null`.

    if (value == other) {
        return INT_1;
    } else {
        die("`totalOrder` called with incompatible arguments.");
    }
}

/** Initializes the module. */
MOD_INIT(Null) {
    MOD_USE(Int);

    CLS_Null = makeCoreClass("Null", CLS_Data,
        NULL,
        NULL);

    METH_BIND(Null, totalEq);
    METH_BIND(Null, totalOrder);

    THE_NULL = datAllocValue(CLS_Null, 0);
    datImmortalize(THE_NULL);
}

// Documented in header.
zvalue CLS_Null = NULL;

// Documented in header.
zvalue THE_NULL = NULL;
