/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Private Definitions
 */

/**
 * Payload data for all Deriv values.
 */
typedef struct {
    /** Data payload. */
    zvalue data;
} DerivInfo;

/**
 * Gets the info of a derived value.
 */
static DerivInfo *getInfo(zvalue value) {
    return (DerivInfo *) pbPayload(value);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue dataFromValue(zvalue value, zvalue secret) {
    zvalue type = value->type;

    if (typeIsDerived(type) && typeSecretIs(type, secret)) {
        return getInfo(value)->data;
    } else {
        return NULL;
    }
}

/* Documented in header. */
zvalue dataOf(zvalue value) {
    return dataFromValue(value, NULL);
}

/* Documented in header. */
zvalue makeDeriv(zvalue type, zvalue data, zvalue secret) {
    pbAssertValid(type);
    pbAssertValidOrNull(data);
    pbAssertValidOrNull(secret);

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
zvalue makeValue(zvalue type, zvalue data) {
    return makeDeriv(type, data, NULL);
}


/*
 * Type Definition
 *
 * **Note:** This isn't the usual form of type definition, since these
 * methods are bound on many types.
 */

/* Documented in header. */
METH_IMPL(Deriv, eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    return pbEq(getInfo(v1)->data, getInfo(v2)->data) ? v2 : NULL;
}

/* Documented in header. */
METH_IMPL(Deriv, gcMark) {
    zvalue value = args[0];
    pbMark(getInfo(value)->data);
    return NULL;
}

/* Documented in header. */
METH_IMPL(Deriv, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zvalue data1 = getInfo(v1)->data;
    zvalue data2 = getInfo(v2)->data;

    if (data1 == NULL) {
        return (data2 == NULL) ? PB_0 : PB_NEG1;
    } else if (data2 == NULL) {
        return PB_1;
    } else {
        return intFromZint(pbOrder(data1, data2));
    }
}

/* Documented in header. */
void derivBind(zvalue type) {
    genericBindCore(GFN_eq,     type, Deriv_eq);
    genericBindCore(GFN_gcMark, type, Deriv_gcMark);
    genericBindCore(GFN_order,  type, Deriv_order);
}
