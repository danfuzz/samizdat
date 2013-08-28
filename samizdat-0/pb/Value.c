/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"

#include <stdio.h>
#include <string.h>


/*
 * Private Definitions
 */

/**
 * The next identity value to return. This starts at `1`, because `0` is
 * taken to mean "uninitialized".
 */
static zint theNextIdentity = 1;

/**
 * Gets a unique "order id" to use when comparing otherwise-incomparable
 * values of the same type, for use in defining the total order of values.
 */
static zint nextIdentity(void) {
    if (theNextIdentity < 0) {
        // At one new identity per nanosecond: (1<<63) nsec ~== 292 years.
        die("Too many identified values!");
    }

    zint result = theNextIdentity;
    theNextIdentity++;
    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue dataOf(zvalue value) {
    return valDataOf(value, NULL);
}

/* Documented in header. */
extern void *pbPayload(zvalue value);

/* Documented in header. */
zint valIdentityOf(zvalue value) {
    if (!typeIsIdentified(typeOf(value))) {
        die("Attempt to use `valIdentityOf` on non-identified value.");
    }

    zint result = value->identity;

    if (result == 0) {
        result = value->identity = nextIdentity();
    }

    return result;
}

/* Documented in header. */
char *valDebugString(zvalue value) {
    if (value == NULL) {
        return strdup("(null)");
    }

    zvalue result = GFN_CALL(debugString, value);
    zint size = utf8SizeFromString(result);
    char str[size + 1];

    utf8FromString(size + 1, str, result);
    return strdup(str);
}

/* Documented in header. */
bool valEq(zvalue v1, zvalue v2) {
    if (v1 == v2) {
        return true;
    } else if ((v1 == NULL) || (v2 == NULL)) {
        return false;
    }

    assertValid(v1);
    assertValid(v2);

    if (haveSameType(v1, v2)) {
        return (GFN_CALL(totEq, v1, v2) != NULL);
    } else {
        return false;
    }
}

/* Documented in header. */
zorder valOrder(zvalue v1, zvalue v2) {
    if (v1 == v2) {
        return ZSAME;
    } else if (v1 == NULL) {
        return ZLESS;
    } else if (v2 == NULL) {
        return ZMORE;
    }

    assertValid(v1);
    assertValid(v2);

    if (haveSameType(v1, v2)) {
        zstackPointer save = pbFrameStart();
        zorder result = zintFromInt(GFN_CALL(totOrder, v1, v2));
        pbFrameReturn(save, NULL);
        return result;
    } else {
        return valOrder(typeOf(v1), typeOf(v2));
    }
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Value, debugString) {
    zvalue value = args[0];
    zvalue type = typeOf(value);
    char addrBuf[19]; // Includes room for "0x" and "\0".

    sprintf(addrBuf, "%p", value);

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@("),
        GFN_CALL(debugString, type),
        stringFromUtf8(-1, " @ "),
        stringFromUtf8(-1, addrBuf),
        stringFromUtf8(-1, ")"));
}

/* Documented in header. */
METH_IMPL(Value, totEq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    if (v1 == v2) {
        return v2;
    }

    zvalue result = GFN_CALL(totOrder, v1, v2);
    return valEq(result, INT_0) ? v1 : NULL;
}

/* Documented in header. */
METH_IMPL(Value, totOrder) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    if (v1 == v2) {
        return INT_0;
    }

    zint id1 = valIdentityOf(v1);
    zint id2 = valIdentityOf(v2);

    if (id1 < id2) {
        return INT_NEG1;
    } else if (id1 > id2) {
        return INT_1;
    } else {
        return INT_0;
    }
}

/* Documented in header. */
void pbBindValue(void) {
    GFN_debugString = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "debugString"));
    pbImmortalize(GFN_debugString);

    GFN_totEq = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "eq"));
    pbImmortalize(GFN_totEq);

    GFN_gcMark = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "gcMark"));
    pbImmortalize(GFN_gcMark);

    GFN_totOrder = makeGeneric(2, 2, GFN_SAME_TYPE, stringFromUtf8(-1, "order"));
    pbImmortalize(GFN_totOrder);

    // Note: The type `Type` is responsible for initializing `TYPE_Value`.

    METH_BIND(Value, debugString);
    METH_BIND(Value, totEq);
    METH_BIND(Value, totOrder);
}

/* Documented in header. */
zvalue TYPE_Value = NULL;

/* Documented in header. */
zvalue GFN_debugString = NULL;

/* Documented in header. */
zvalue GFN_gcMark = NULL;

/* Documented in header. */
zvalue GFN_totEq = NULL;

/* Documented in header. */
zvalue GFN_totOrder = NULL;
