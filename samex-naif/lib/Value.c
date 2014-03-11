/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
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

    return valEq(v1, v2) ? v2 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(ge) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) >= 0) ? v2 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(gt) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) > 0) ? v2 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(hasType) {
    zvalue value = args[0];
    zvalue type = args[1];

    return hasType(value, type) ? value : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(le) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) <= 0) ? v2 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(lt) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return (valOrder(v1, v2) < 0) ? v2 : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(makeValue) {
    zvalue type = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    // TODO: Remove this when types-per-se are strictly required.
    if (!hasType(type, TYPE_Type)) {
        type = typeFromName(type);
    }

    return makeValue_new(type, value, NULL);
}

/* Documented in spec. */
FUN_IMPL_DECL(ne) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return valEq(v1, v2) ? NULL : v2;
}

/* Documented in spec. */
FUN_IMPL_DECL(typeOf) {
    zvalue result = typeOf(args[0]);

    // TODO: Remove this when the high level model specifies types-per-se.
    if (typeIsTransparentDerived(result)) {
        return nameOf(result);
    }

    return result;
}
