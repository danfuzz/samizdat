/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Module functions
 */

/* Documented in header. */
zvalue Transparent_dataOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    return ((TransparentInfo *) pbPayload(value))->data;
}

/* Documented in header. */
zvalue Transparent_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    pbMark(((TransparentInfo *) pbPayload(value))->data);
    return NULL;
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
