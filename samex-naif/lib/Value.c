/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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

/* Documented in spec. */
FUN_IMPL_DECL(dataOf) {
    zvalue value = args[0];
    zvalue secret = (argCount == 2) ? args[1] : NULL;

    return valDataOf(value, secret);
}

/* Documented in spec. */
FUN_IMPL_DECL(eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return valEq(v1, v2) ? v1 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(hasType) {
    zvalue value = args[0];
    zvalue type = args[1];

    return hasType(value, type) ? value : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(makeValue) {
    zvalue type = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    return makeValue(type, value, NULL);
}

/* Documented in spec. */
FUN_IMPL_DECL(order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return intFromZint(valOrder(v1, v2));
}
