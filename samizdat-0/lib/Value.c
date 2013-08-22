/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/Int.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Does most of the work of `totalOrderIs`.
 */
static bool doOrderIs(zint argCount, const zvalue *args) {
    zorder want = zintFromInt(args[2]);

    if ((argCount == 3) && (want == ZSAME)) {
        return valEq(args[0], args[1]);
    }

    zorder comp = valOrder(args[0], args[1]);

    return (comp == want) ||
        ((argCount == 4) && (comp == zintFromInt(args[3])));
}


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(dataOf) {
    zvalue value = args[0];
    zvalue secret = (argCount == 2) ? args[1] : NULL;

    return valDataOf(value, secret);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(isOpaqueValue) {
    zvalue value = args[0];
    return hasType(typeOf(value), TYPE_Type) ? value : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeValue) {
    zvalue value = (argCount == 2) ? args[1] : NULL;
    return makeTransValue(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(totalOrder) {
    return intFromZint(valOrder(args[0], args[1]));
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
