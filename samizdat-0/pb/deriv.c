/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Gets the info of a derived value.
 */
static DerivInfo *derivInfo(zvalue value) {
    return (DerivInfo *) pbPayload(value);
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue Deriv_eq(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return pbEq(derivInfo(v1)->data, derivInfo(v2)->data) ? v2 : NULL;
}

/* Documented in header. */
zvalue Deriv_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    pbMark(derivInfo(value)->data);
    return NULL;
}

/* Documented in header. */
zvalue Deriv_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return intFromZint(pbOrder(derivInfo(v1)->data, derivInfo(v2)->data));
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue dataFromValue(zvalue value, zvalue secret) {
    zvalue type = value->type;

    if (typeIsDerived(type) && typeSecretIs(type, secret)) {
        return derivInfo(value)->data;
    } else {
        return NULL;
    }
}

/* Documented in header. */
zvalue pbDataOf(zvalue value) {
    return dataFromValue(value, NULL);
}

/* Documented in header. */
zvalue derivFrom(zvalue type, zvalue data, zvalue secret) {
    pbAssertValid(type);
    pbAssertValidOrNull(data);
    pbAssertValidOrNull(secret);

    if (pbTypeOf(type) != TYPE_Type) {
        type = transparentTypeFromName(type);
    }

    // Make sure the secrets match. In the case of a transparent type,
    // this checks that `secret` is `NULL`.
    if (!typeSecretIs(type, secret)) {
        die("Attempt to create derived value with incorrect secret.");
    }

    zvalue result = pbAllocValue(type, sizeof(DerivInfo));
    ((DerivInfo *) pbPayload(result))->data = data;

    return result;
}

/* Documented in header. */
zvalue valueFrom(zvalue type, zvalue data) {
    return derivFrom(type, data, NULL);
}
