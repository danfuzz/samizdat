/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Does the type assertions that are part of `coreOrder` and `coreOrderIs`.
 */
static void coreOrderTypeCheck(zvalue v1, zvalue v2) {
    assertHaveSameType(v1, v2);

    if (!pbEq(typeOf(v1), typeOf(v2))) {
        die("Mismatched derived types.");
    }
}

/**
 * Does most of the work of `coreOrderIs` and `totalOrderIs`.
 */
static bool doOrderIs(zint argCount, const zvalue *args) {
    zorder want = zintFromInt(args[2]);

    if ((argCount == 3) && (want == ZSAME)) {
        return pbEq(args[0], args[1]);
    }

    zorder comp = pbOrder(args[0], args[1]);

    return (comp == want) ||
        ((argCount == 4) && (comp == zintFromInt(args[3])));
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(coreOrder) {
    zvalue arg0 = args[0];
    zvalue arg1 = args[1];

    coreOrderTypeCheck(arg0, arg1);
    return intFromZint(pbOrder(arg0, arg1));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(coreOrderIs) {
    zvalue arg0 = args[0];
    zvalue arg1 = args[1];

    coreOrderTypeCheck(arg0, arg1);
    return doOrderIs(argCount, args) ? arg1 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(dataOf) {
    zvalue value = args[0];
    zvalue secret = (argCount == 2) ? args[1] : NULL;

    return dataFromValue(value, secret);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(isOpaqueValue) {
    zvalue value = args[0];
    return hasType(typeOf(value), TYPE_Type) ? value : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(totalOrder) {
    return intFromZint(pbOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(totalOrderIs) {
    return doOrderIs(argCount, args) ? args[1] : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(typeName) {
    return typeName(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(typeOf) {
    return typeOf(args[0]);
}
