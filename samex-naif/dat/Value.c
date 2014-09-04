// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>
#include <string.h>

#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
extern zvalue datNonVoid(zvalue value);

// Documented in header.
void datNonVoidError(void) {
    die("Attempt to use void in non-void context.");
}

// Documented in header.
extern void *datPayload(zvalue value);

// Documented in header.
zvalue get_class(zvalue value) {
    return value->cls;
}

// Documented in header.
char *valDebugString(zvalue value) {
    if (value == NULL) {
        return utilStrdup("(null)");
    }

    if (SYM_NAME(debugString) == NULL) {
        die("Too early to call `debugString`.");
    }

    return utf8DupFromString(METH_CALL(debugString, value));
}

// Documented in header.
zvalue valEq(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `valEq`.");
    } else if (value == other) {
        return value;
    } else if (haveSameClass(value, other)) {
        return (METH_CALL(totalEq, value, other) != NULL) ? value : NULL;
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
        return INT_0;
    }

    zvalue valueCls = get_class(value);
    zvalue otherCls = get_class(other);

    if (valueCls == otherCls) {
        return METH_CALL(totalOrder, value, other);
    } else {
        return METH_CALL(totalOrder, valueCls, otherCls);
    }
}

// Documented in header.
zvalue valOrderNullOk(zvalue value, zvalue other) {
    if (value == other) {
        return INT_0;
    } else if (value == NULL) {
        return INT_NEG1;
    } else if (other == NULL) {
        return INT_1;
    } else {
        return valOrder(value, other);
    }
}

// Documented in header.
zvalue valToString(zvalue value) {
    return METH_CALL(toString, value);
}

// Documented in header.
zorder valZorder(zvalue value, zvalue other) {
    // This frame usage avoids having the `zvalue` result of the call pollute
    // the stack. See note on `valOrder` for more color.
    zstackPointer save = datFrameStart();
    zvalue result = valOrder(value, other);

    if (result == NULL) {
        die("Attempt to order unordered values.");
    }

    zorder order = zintFromInt(result);
    datFrameReturn(save, NULL);
    return order;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL_0(Value, debugString) {
    zvalue cls = get_class(ths);
    zvalue name = METH_CALL(debugSymbol, ths);
    char addrBuf[19];  // Includes room for `0x` and `\0`.

    if (name == NULL) {
        name = EMPTY_STRING;
    } else if (!hasClass(name, CLS_Symbol)) {
        // Suppress a non-symbol name.
        name = stringFromUtf8(-1, " (non-symbol name)");
    } else {
        name = METH_CALL(cat, stringFromUtf8(-1, " "), valToString(name));
    }

    sprintf(addrBuf, "%p", ths);

    return METH_CALL(cat,
        stringFromUtf8(-1, "@("),
        METH_CALL(debugString, cls),
        name,
        stringFromUtf8(-1, " @ "),
        stringFromUtf8(-1, addrBuf),
        stringFromUtf8(-1, ")"));
}

// Documented in header.
METH_IMPL_0(Value, debugSymbol) {
    return NULL;
}

// Documented in header.
METH_IMPL_0(Value, gcMark) {
    // Nothing to do.
    return NULL;
}

// Documented in header.
METH_IMPL_1(Value, perEq, other) {
    return valEq(ths, other);
}

// Documented in header.
METH_IMPL_1(Value, perOrder, other) {
    return valOrder(ths, other);
}

// Documented in header.
METH_IMPL_1(Value, totalEq, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`totalEq` called with incompatible arguments.");
    }

    return (ths == other) ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(Value, totalOrder, other) {
    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`totalOrder` called with incompatible arguments.");
    }

    return valEq(ths, other) ? INT_0 : NULL;
}

/** Initializes the module. */
MOD_INIT(Value) {
    MOD_USE(objectModel);
    MOD_USE_NEXT(call);

    // Initializing `Value` also initializes the rest of the core classes.
    // This also gets all the protocols indirectly via their implementors.
    MOD_USE_NEXT(Class);
    MOD_USE_NEXT(Symbol);
    MOD_USE_NEXT(SymbolTable);
    MOD_USE_NEXT(Object);
    MOD_USE_NEXT(Data);
    MOD_USE_NEXT(DerivedData);
    MOD_USE_NEXT(Builtin);
    MOD_USE_NEXT(Int);
    MOD_USE_NEXT(Jump);
    MOD_USE_NEXT(List);
    MOD_USE_NEXT(Null);
    MOD_USE_NEXT(String);

    SYM_INIT(call);
    SYM_INIT(debugString);
    SYM_INIT(debugSymbol);
    SYM_INIT(exports);
    SYM_INIT(imports);
    SYM_INIT(gcMark);
    SYM_INIT(main);
    SYM_INIT(perEq);
    SYM_INIT(perOrder);
    SYM_INIT(resources);
    SYM_INIT(toString);
    SYM_INIT(totalEq);
    SYM_INIT(totalOrder);

    // Note: The `objectModel` module initializes `CLS_Value`.
    classBindMethods(CLS_Value,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Value, debugString),
            METH_BIND(Value, debugSymbol),
            METH_BIND(Value, gcMark),
            METH_BIND(Value, perEq),
            METH_BIND(Value, perOrder),
            METH_BIND(Value, totalEq),
            METH_BIND(Value, totalOrder),
            NULL));
}

// Documented in header.
zvalue CLS_Value = NULL;

// Documented in header.
SYM_DEF(call);

// Documented in header.
SYM_DEF(debugString);

// Documented in header.
SYM_DEF(debugSymbol);

// Documented in header.
SYM_DEF(exports);

// Documented in header.
SYM_DEF(imports);

// Documented in header.
SYM_DEF(gcMark);

// Documented in header.
SYM_DEF(main);

// Documented in header.
SYM_DEF(perEq);

// Documented in header.
SYM_DEF(perOrder);

// Documented in header.
SYM_DEF(resources);

// Documented in header.
SYM_DEF(toString);

// Documented in header.
SYM_DEF(totalEq);

// Documented in header.
SYM_DEF(totalOrder);
