// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>
#include <string.h>

#include "type/Int.h"
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

    if (SEL_NAME(debugString) == NULL) {
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
METH_IMPL(Value, debugName) {
    return NULL;
}

// Documented in header.
METH_IMPL(Value, debugString) {
    zvalue value = args[0];
    zvalue cls = get_class(value);
    zvalue name = METH_CALL(debugName, value);
    char addrBuf[19];  // Includes room for `0x` and `\0`.

    if (name == NULL) {
        name = EMPTY_STRING;
    } else if (!hasClass(name, CLS_String)) {
        // Suppress a non-string name.
        name = stringFromUtf8(-1, "(non-string name)");
    } else {
        name = METH_CALL(cat,
            stringFromUtf8(-1, " "),
            METH_CALL(debugString, name));
    }

    sprintf(addrBuf, "%p", value);

    return METH_CALL(cat,
        stringFromUtf8(-1, "@("),
        METH_CALL(debugString, cls),
        name,
        stringFromUtf8(-1, " @ "),
        stringFromUtf8(-1, addrBuf),
        stringFromUtf8(-1, ")"));
}

// Documented in header.
METH_IMPL(Value, gcMark) {
    // Nothing to do.
    return NULL;
}

// Documented in header.
METH_IMPL(Value, perEq) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valEq(value, other);
}

// Documented in header.
METH_IMPL(Value, perOrder) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valOrder(value, other);
}

// Documented in header.
METH_IMPL(Value, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to have the same class.

    if (!haveSameClass(value, other)) {
        die("`totalEq` called with incompatible arguments.");
    }

    return (value == other) ? value : NULL;
}

// Documented in header.
METH_IMPL(Value, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to have the same class.

    if (!haveSameClass(value, other)) {
        die("`totalOrder` called with incompatible arguments.");
    }

    return valEq(value, other) ? INT_0 : NULL;
}

/** Initializes the module. */
MOD_INIT(Value) {
    MOD_USE(objectModel);

    // Initializing `Value` also initializes the rest of the core classes.
    // This also gets all the protocols indirectly via their implementors.
    MOD_USE_NEXT(Class);
    MOD_USE_NEXT(Selector);
    MOD_USE_NEXT(Object);
    MOD_USE_NEXT(Data);
    MOD_USE_NEXT(DerivedData);
    MOD_USE_NEXT(Builtin);
    MOD_USE_NEXT(Generic);
    MOD_USE_NEXT(Int);
    MOD_USE_NEXT(Jump);
    MOD_USE_NEXT(List);
    MOD_USE_NEXT(Null);
    MOD_USE_NEXT(String);
    MOD_USE_NEXT(Uniqlet);

    // Note: The `objectModel` module initializes `CLS_Value`.

    SEL_INIT(debugName);
    SEL_INIT(debugString);
    SEL_INIT(gcMark);
    SEL_INIT(perEq);
    SEL_INIT(perOrder);
    SEL_INIT(totalEq);
    SEL_INIT(totalOrder);

    METH_BIND(Value, debugName);
    METH_BIND(Value, debugString);
    METH_BIND(Value, gcMark);
    METH_BIND(Value, perEq);
    METH_BIND(Value, perOrder);
    METH_BIND(Value, totalEq);
    METH_BIND(Value, totalOrder);
}

// Documented in header.
zvalue CLS_Value = NULL;

// Documented in header.
SEL_DEF(debugName);

// Documented in header.
SEL_DEF(debugString);

// Documented in header.
SEL_DEF(gcMark);

// Documented in header.
SEL_DEF(perEq);

// Documented in header.
SEL_DEF(perOrder);

// Documented in header.
SEL_DEF(totalEq);

// Documented in header.
SEL_DEF(totalOrder);
