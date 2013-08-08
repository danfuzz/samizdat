/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Array of cached type-only values, with single-character strings for
 * types, for low character codes. These are used as charecter "tokens"
 * when doing tokenization, hence the name.
 */
static zvalue CACHED_CHAR_TOKENS[PB_MAX_CACHED_CHAR + 1];

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

    return pbNullSafeEq(derivInfo(v1)->data, derivInfo(v2)->data) ? v2 : NULL;
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
    zvalue data1 = derivInfo(v1)->data;
    zvalue data2 = derivInfo(v2)->data;

    if (data1 == NULL) {
        return (data2 == NULL) ? PB_0 : PB_NEG1;
    } else if (data2 == NULL) {
        return PB_1;
    } else {
        return intFromZint(pbOrder(data1, data2));
    }
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
zvalue dataOf(zvalue value) {
    return dataFromValue(value, NULL);
}

/* Documented in header. */
zvalue derivFrom(zvalue type, zvalue data, zvalue secret) {
    pbAssertValid(type);
    pbAssertValidOrNull(data);
    pbAssertValidOrNull(secret);

    zchar cacheChar = ~0;

    if (type->type != TYPE_Type) {
        if ((data == NULL) &&
            (secret == NULL) &&
            (type->type == TYPE_String) &&
            (pbSize(type) == 1)) {
            cacheChar = stringNth(type, 0);
            if (cacheChar <= PB_MAX_CACHED_CHAR) {
                zvalue result = CACHED_CHAR_TOKENS[cacheChar];
                if (result != NULL) {
                    return result;
                }
            }
        }
        type = transparentTypeFromName(type);
    }

    // Make sure the secrets match. In the case of a transparent type,
    // this checks that `secret` is `NULL`.
    if (!typeSecretIs(type, secret)) {
        die("Attempt to create derived value with incorrect secret.");
    }

    zvalue result = pbAllocValue(type, sizeof(DerivInfo));
    ((DerivInfo *) pbPayload(result))->data = data;

    if (cacheChar != ~0) {
        CACHED_CHAR_TOKENS[cacheChar] = result;
        pbImmortalize(result);
    }

    return result;
}

/* Documented in header. */
zvalue valueFrom(zvalue type, zvalue data) {
    return derivFrom(type, data, NULL);
}
