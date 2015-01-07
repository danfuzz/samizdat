// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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

// Documented in spec.
METH_IMPL_1(Null, crossEq, other) {
    // Note: `other` not guaranteed to be `null`.
    if (ths == other) {
        return ths;
    } else {
        die("`crossEq` called with incompatible arguments.");
    }
}

// Documented in spec.
METH_IMPL_1(Null, crossOrder, other) {
    // Note: `other` not guaranteed to be `null`.
    if (ths == other) {
        return SYM(same);
    } else {
        die("`crossOrder` called with incompatible arguments.");
    }
}

// Documented in spec.
METH_IMPL_0(Null, debugString) {
    return stringFromUtf8(-1, "null");
}

/** Initializes the module. */
MOD_INIT(Null) {
    MOD_USE(Value);

    CLS_Null = makeCoreClass(SYM(Null), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Null, crossEq),
            METH_BIND(Null, crossOrder),
            METH_BIND(Null, debugString)));

    THE_NULL = datImmortalize(datAllocValue(CLS_Null, 0));
}

// Documented in header.
zvalue CLS_Null = NULL;

// Documented in header.
zvalue THE_NULL = NULL;
