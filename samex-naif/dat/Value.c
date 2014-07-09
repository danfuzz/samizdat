// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdio.h>
#include <string.h>

#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * The next identity value to return. This starts at `1`, because `0` is
 * taken to mean "uninitialized."
 */
static zint theNextSelfId = 1;

/**
 * Gets a unique "self-identity" value to use when comparing
 * otherwise-incomparable values of the same type, for use in defining the
 * total order of values.
 */
static zint nextSelfId(void) {
    if (theNextSelfId < 0) {
        // At one new identity per nanosecond: (1<<63) nsec ~== 292 years.
        die("Too many selfish values!");
    }

    zint result = theNextSelfId;
    theNextSelfId++;
    return result;
}


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
zvalue get_type(zvalue value) {
    return value->type;
}

// Documented in header.
zint valSelfIdOf(zvalue value) {
    if (!typeIsSelfish(get_type(value))) {
        die("Attempt to use `valSelfIdOf` on non-selfish value.");
    }

    zint result = value->selfId;

    if (result == 0) {
        result = value->selfId = nextSelfId();
    }

    return result;
}

// Documented in header.
char *valDebugString(zvalue value) {
    if (value == NULL) {
        return utilStrdup("(null)");
    }

    return utf8DupFromString(GFN_CALL(debugString, value));
}

// Documented in header.
zvalue valEq(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `valEq`.");
    } else if (value == other) {
        return value;
    } else if (haveSameType(value, other)) {
        return (GFN_CALL(totalEq, value, other) != NULL) ? value : NULL;
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

    zvalue valueType = get_type(value);
    zvalue otherType = get_type(other);

    if (valueType == otherType) {
        return GFN_CALL(totalOrder, value, other);
    } else {
        return GFN_CALL(totalOrder, valueType, otherType);
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
    zorder result = zintFromInt(valOrder(value, other));
    datFrameReturn(save, NULL);
    return result;
}


//
// Type Definition
//

// Documented in header.
METH_IMPL(Value, debugName) {
    return NULL;
}

// Documented in header.
METH_IMPL(Value, debugString) {
    zvalue value = args[0];
    zvalue type = get_type(value);
    zvalue name = GFN_CALL(debugName, value);
    char addrBuf[19];  // Includes room for `0x` and `\0`.

    if (name == NULL) {
        name = EMPTY_STRING;
    } else if (!hasType(name, TYPE_String)) {
        // Suppress a non-string name.
        name = stringFromUtf8(-1, "(non-string name)");
    } else {
        name = GFN_CALL(cat,
            stringFromUtf8(-1, " "),
            GFN_CALL(debugString, name));
    }

    sprintf(addrBuf, "%p", value);

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@("),
        GFN_CALL(debugString, type),
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
    return funCall(GFN_totalEq, argCount, args);
}

// Documented in header.
METH_IMPL(Value, perOrder) {
    return funCall(GFN_totalOrder, argCount, args);
}

// Documented in header.
METH_IMPL(Value, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];

    if (value == other) {
        return other;
    }

    zvalue result = GFN_CALL(totalOrder, value, other);
    return (valEq(result, INT_0) != NULL) ? value : NULL;
}

// Documented in header.
METH_IMPL(Value, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];

    if (value == other) {
        return INT_0;
    }

    zint id1 = valSelfIdOf(value);
    zint id2 = valSelfIdOf(other);

    if (id1 < id2) {
        return INT_NEG1;
    } else if (id1 > id2) {
        return INT_1;
    } else {
        return INT_0;
    }
}

/** Initializes the module. */
MOD_INIT(Value) {
    MOD_USE(typeSystem);

    // Initializing `Value` also initializes the rest of the core types.
    // This also gets all the protocols indirectly via their implementors.
    MOD_USE_NEXT(Data);
    MOD_USE_NEXT(Type);
    MOD_USE_NEXT(String);
    MOD_USE_NEXT(Builtin);
    MOD_USE_NEXT(Generic);
    MOD_USE_NEXT(Jump);
    MOD_USE_NEXT(Int);
    MOD_USE_NEXT(Uniqlet);
    MOD_USE_NEXT(List);
    MOD_USE_NEXT(DerivedData);

    // Note: The `typeSystem` module initializes `TYPE_Value`.

    GFN_debugName = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "debugName"));
    datImmortalize(GFN_debugName);

    GFN_debugString =
        makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "debugString"));
    datImmortalize(GFN_debugString);

    GFN_gcMark = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "gcMark"));
    datImmortalize(GFN_gcMark);

    GFN_perEq = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "perEq"));
    datImmortalize(GFN_perEq);

    GFN_perOrder = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "perOrder"));
    datImmortalize(GFN_perOrder);

    GFN_totalEq =
        makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "totalEq"));
    datImmortalize(GFN_totalEq);

    GFN_totalOrder =
        makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "totalOrder"));
    datImmortalize(GFN_totalOrder);

    METH_BIND(Value, debugName);
    METH_BIND(Value, debugString);
    METH_BIND(Value, gcMark);
    METH_BIND(Value, perEq);
    METH_BIND(Value, perOrder);
    METH_BIND(Value, totalEq);
    METH_BIND(Value, totalOrder);
}

// Documented in header.
zvalue TYPE_Value = NULL;

// Documented in header.
zvalue GFN_debugName = NULL;

// Documented in header.
zvalue GFN_debugString = NULL;

// Documented in header.
zvalue GFN_gcMark = NULL;

// Documented in header.
zvalue GFN_perEq = NULL;

// Documented in header.
zvalue GFN_perOrder = NULL;

// Documented in header.
zvalue GFN_totalEq = NULL;

// Documented in header.
zvalue GFN_totalOrder = NULL;
