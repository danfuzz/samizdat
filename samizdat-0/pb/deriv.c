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
static TransparentInfo *derivInfo(zvalue value) {
    return (TransparentInfo *) pbPayload(value);
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue Transparent_dataOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    zvalue secret = (argCount == 2) ? args[1] : NULL;

    if (typeSecretIs(value->type, secret)) {
        return derivInfo(value)->data;
    } else {
        return NULL;
    }
}

/* Documented in header. */
zvalue Transparent_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    pbMark(derivInfo(value)->data);
    return NULL;
}

/* Documented in header. */
zvalue Transparent_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return intFromZint(pbOrder(derivInfo(v1)->data, derivInfo(v2)->data));
}


/*
 * Exported functions
 */

// TODO: This is now misnamed. It makes general transparent values.
zvalue derivFrom(zvalue type, zvalue data) {
    pbAssertValid(type);

    if (pbTypeOf(type) == TYPE_Type) {
        // This should probably be fixed.
        die("Can't create transparent value given type value.");
    }

    if (data != NULL) {
        pbAssertValid(data);
    }

    // Get a `Type` per se. `NULL` indicates it is to be transparent.
    type = typeFrom(type, NULL);
    zvalue result = pbAllocValue(type, sizeof(TransparentInfo));
    ((TransparentInfo *) pbPayload(result))->data = data;

    return result;
}
