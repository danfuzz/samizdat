// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>

#include "type/Cmp.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// This provides the non-inline version of this function.
extern zvalue datNonVoid(zvalue value);

// Documented in header.
void datNonVoidError(void) {
    die("Attempt to use void in non-void context.");
}

// This provides the non-inline version of this function.
extern void *datPayload(zvalue value);


//
// Class Definition
//

// Documented in spec.
METH_IMPL_1(Value, castToward, cls) {
    return typeAccepts(cls, ths) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Value, crossEq, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`crossEq` called with incompatible arguments.");
    }

    return (ths == other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Value, crossOrder, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`crossOrder` called with incompatible arguments.");
    }

    return cmpEq(ths, other) ? SYM(same) : NULL;
}

// Documented in spec.
METH_IMPL_0(Value, debugString) {
    zvalue cls = classOf(ths);
    zvalue name = METH_CALL(ths, debugSymbol);
    char addrBuf[19];  // Includes room for `0x` and `\0`.

    if (name == NULL) {
        name = EMPTY_STRING;
    } else if (!typeAccepts(CLS_Symbol, name)) {
        // Suppress a non-symbol name.
        name = stringFromUtf8(-1, " (non-symbol name)");
    } else {
        name = cm_cat(stringFromUtf8(-1, " "), name);
    }

    sprintf(addrBuf, "%p", ths);

    return cm_cat(
        stringFromUtf8(-1, "@<"),
        METH_CALL(cls, debugString),
        name,
        stringFromUtf8(-1, " @ "),
        stringFromUtf8(-1, addrBuf),
        stringFromUtf8(-1, ">"));
}

// Documented in spec.
METH_IMPL_0(Value, debugSymbol) {
    return NULL;
}

// Documented in spec.
METH_IMPL_1(Value, perEq, other) {
    return cmpEq(ths, other);
}

// Documented in spec.
METH_IMPL_1(Value, perOrder, other) {
    return cmpOrder(ths, other);
}

// Documented in header.
void bindMethodsForValue(void) {
    classBindMethods(CLS_Value,
        NULL,
        METH_TABLE(
            METH_BIND(Value, castToward),
            METH_BIND(Value, crossEq),
            METH_BIND(Value, crossOrder),
            METH_BIND(Value, debugString),
            METH_BIND(Value, debugSymbol),
            METH_BIND(Value, perEq),
            METH_BIND(Value, perOrder)));
}

/** Initializes the module. */
MOD_INIT(Value) {
    MOD_USE(objectModel);

    // Initializing `Value` also initializes the rest of the core classes.
    // This also gets all the protocols indirectly via their implementors.

    MOD_USE_NEXT(Class);
    MOD_USE_NEXT(Symbol);
    MOD_USE_NEXT(SymbolTable);
    MOD_USE_NEXT(Record);

    MOD_USE_NEXT(Builtin);
    MOD_USE_NEXT(Box);
    MOD_USE_NEXT(Cmp);
    MOD_USE_NEXT(Int);
    MOD_USE_NEXT(List);
    MOD_USE_NEXT(String);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForValue()`.
}

// Documented in header.
zvalue CLS_Value = NULL;
