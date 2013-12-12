/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Int.h"
#include "type/Type.h"
#include "type/Value.h"


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
PRIM_IMPL(eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return valEq(v1, v2) ? v2 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ge) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) >= 0) ? v2 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(gt) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) > 0) ? v2 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(hasType) {
    zvalue value = args[0];
    zvalue type = args[1];

    return hasType(value, type) ? value : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(le) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) <= 0) ? v2 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lt) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) < 0) ? v2 : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeValue) {
    zvalue value = (argCount == 2) ? args[1] : NULL;
    return makeTransValue(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ne) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return valEq(v1, v2) ? NULL : v2;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(totalOrder) {
    return intFromZint(valOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(typeOf) {
    return typeOf(args[0]);
}
