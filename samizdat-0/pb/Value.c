/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Callable.h"
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

    pbAssertValid(v1);
    pbAssertValid(v2);

    if (haveSameType(v1, v2)) {
        return (GFN_CALL(eq, v1, v2) != NULL);
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

    pbAssertValid(v1);
    pbAssertValid(v2);

    if (haveSameType(v1, v2)) {
        zstackPointer save = pbFrameStart();
        zorder result = zintFromInt(GFN_CALL(order, v1, v2));
        pbFrameReturn(save, NULL);
        return result;
    } else {
        return valOrder(typeOf(v1), typeOf(v2));
    }
}

/* Documented in header. */
zint valSize(zvalue value) {
    return zintFromInt(GFN_CALL(size, value));
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

    zvalue result = stringFromUtf8(-1, "@(");
    result = stringCat(result, GFN_CALL(debugString, type));
    result = stringCat(result, stringFromUtf8(-1, " @ "));
    result = stringCat(result, stringFromUtf8(-1, addrBuf));
    result = stringCat(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
METH_IMPL(Value, eq) {
    return NULL;
}

/* Documented in header. */
METH_IMPL(Value, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zint id1 = valIdentityOf(v1);
    zint id2 = valIdentityOf(v2);

    if (id1 < id2) {
        return PB_NEG1;
    } else if (id1 > id2) {
        return PB_1;
    } else {
        return PB_0;
    }
}

/* Documented in header. */
METH_IMPL(Value, size) {
    return intFromZint(0);
}

/* Documented in header. */
void pbBindValue(void) {
    GFN_debugString = makeGeneric(1, 1, stringFromUtf8(-1, "debugString"));
    pbImmortalize(GFN_debugString);

    GFN_eq = makeGeneric(2, 2, stringFromUtf8(-1, "eq"));
    pbImmortalize(GFN_eq);

    GFN_gcMark = makeGeneric(1, 1, stringFromUtf8(-1, "gcMark"));
    pbImmortalize(GFN_gcMark);

    GFN_order = makeGeneric(2, 2, stringFromUtf8(-1, "order"));
    pbImmortalize(GFN_order);

    GFN_size = makeGeneric(1, 1, stringFromUtf8(-1, "size"));
    pbImmortalize(GFN_size);

    // Note: The type `Type` is responsible for initializing `TYPE_Value`.

    METH_BIND(Value, debugString);
    METH_BIND(Value, eq);
    METH_BIND(Value, order);
    METH_BIND(Value, size);
}

/* Documented in header. */
zvalue TYPE_Value = NULL;

/* Documented in header. */
zvalue GFN_debugString = NULL;

/* Documented in header. */
zvalue GFN_eq = NULL;

/* Documented in header. */
zvalue GFN_gcMark = NULL;

/* Documented in header. */
zvalue GFN_order = NULL;

/* Documented in header. */
zvalue GFN_size = NULL;
