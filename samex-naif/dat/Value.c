// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>

#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// This provides the non-inline version of this function.
extern zvalue classOf(zvalue value);

// This provides the non-inline version of this function.
extern zvalue datNonVoid(zvalue value);

// Documented in header.
void datNonVoidError(void) {
    die("Attempt to use void in non-void context.");
}

// This provides the non-inline version of this function.
extern void *datPayload(zvalue value);

// Documented in header.
zvalue valEq(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `valEq`.");
    } else if (value == other) {
        return value;
    } else if (haveSameClass(value, other)) {
        return (METH_CALL(value, totalEq, other) != NULL) ? value : NULL;
    } else {
        return NULL;
    }
}

// Documented in header.
bool valEqNullOk(zvalue value, zvalue other) {
    if (value == other) {
        return true;
    } else if ((value == NULL) || (other == NULL)) {
        return false;
    } else {
        return valEq(value, other) != NULL;
    }
}

// Documented in header.
zvalue valOrder(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `valOrder`.");
    } else if (value == other) {
        return SYM(same);
    }

    zvalue valueCls = classOf(value);
    zvalue otherCls = classOf(other);

    if (valueCls == otherCls) {
        return METH_CALL(value, totalOrder, other);
    } else {
        return METH_CALL(valueCls, perOrder, otherCls);
    }
}


//
// Class Definition
//

// Documented in spec.
METH_IMPL_1(Value, castToward, cls) {
    return classAccepts(cls, ths) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_0(Value, debugString) {
    zvalue cls = classOf(ths);
    zvalue name = METH_CALL(ths, debugSymbol);
    char addrBuf[19];  // Includes room for `0x` and `\0`.

    if (name == NULL) {
        name = EMPTY_STRING;
    } else if (!classAccepts(CLS_Symbol, name)) {
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
    return valEq(ths, other);
}

// Documented in spec.
METH_IMPL_1(Value, perOrder, other) {
    return valOrder(ths, other);
}

// Documented in spec.
METH_IMPL_1(Value, totalEq, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`totalEq` called with incompatible arguments.");
    }

    return (ths == other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Value, totalOrder, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`totalOrder` called with incompatible arguments.");
    }

    return valEq(ths, other) ? SYM(same) : NULL;
}

// Documented in header.
void bindMethodsForValue(void) {
    classBindMethods(CLS_Value,
        NULL,
        METH_TABLE(
            METH_BIND(Value, castToward),
            METH_BIND(Value, debugString),
            METH_BIND(Value, debugSymbol),
            METH_BIND(Value, perEq),
            METH_BIND(Value, perOrder),
            METH_BIND(Value, totalEq),
            METH_BIND(Value, totalOrder)));
}

/** Initializes the module. */
MOD_INIT(Value) {
    MOD_USE(objectModel);

    // Initializing `Value` also initializes the rest of the core classes.
    // This also gets all the protocols indirectly via their implementors.
    MOD_USE_NEXT(Class);
    MOD_USE_NEXT(Symbol);
    MOD_USE_NEXT(SymbolTable);
    MOD_USE_NEXT(Object);
    MOD_USE_NEXT(Record);
    MOD_USE_NEXT(Builtin);
    MOD_USE_NEXT(Int);
    MOD_USE_NEXT(List);
    MOD_USE_NEXT(Null);
    MOD_USE_NEXT(String);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForValue()`.
}

// Documented in header.
zvalue CLS_Value = NULL;
