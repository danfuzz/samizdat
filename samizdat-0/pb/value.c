/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stdio.h>


/*
 * Exported Definitions
 */

/* Documented in header. */
zint identityOf(zvalue value) {
    if (!typeIsIdentified(typeOf(value))) {
        die("Attempt to use `identityOf` on non-identified value.");
    }

    zint result = value->identity;

    if (result == 0) {
        result = value->identity = pbOrderId();
    }

    return result;
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
    result = stringAdd(result, funCall(GFN_debugString, 1, &type));
    result = stringAdd(result, stringFromUtf8(-1, " @ "));
    result = stringAdd(result, stringFromUtf8(-1, addrBuf));
    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
METH_IMPL(Value, eq) {
    return NULL;
}

/* Documented in header. */
METH_IMPL(Value, size) {
    return intFromZint(0);
}

/* Documented in header. */
void pbBindValue(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Value`.

    METH_BIND(Value, debugString);
    METH_BIND(Value, eq);
    METH_BIND(Value, size);
}

/* Documented in header. */
zvalue TYPE_Value = NULL;
