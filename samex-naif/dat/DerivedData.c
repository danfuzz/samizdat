/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "type/Builtin.h"
#include "type/Collection.h"
#include "type/DerivedData.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"

#include "impl.h"


/*
 * Private Definitions
 */

/**
 * Payload data for all derived values.
 */
typedef struct {
    /** Data payload. */
    zvalue data;
} DerivedDataInfo;

/**
 * Gets the info of a derived value.
 */
static DerivedDataInfo *getInfo(zvalue value) {
    return (DerivedDataInfo *) datPayload(value);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue valDataOf(zvalue value, zvalue secret) {
    zvalue type = get_type(value);

    if (typeIsDerived(type) && typeHasSecret(type, secret)) {
        return getInfo(value)->data;
    } else {
        return NULL;
    }
}

/* Documented in header. */
zvalue makeValue(zvalue type, zvalue data, zvalue secret) {
    assertValidOrNull(data);
    assertValidOrNull(secret);

    if (!typeHasSecret(type, secret)) {
        die("Attempt to create derived value with incorrect secret.");
    }

    zvalue result = datAllocValue(type, sizeof(DerivedDataInfo));
    ((DerivedDataInfo *) datPayload(result))->data = data;

    return result;
}


/*
 * Type Definition
 *
 * **Note:** This isn't the usual form of type definition, since these
 * methods are bound on many types.
 */

/* Documented in header. */
METH_IMPL(DerivedData, gcMark) {
    zvalue value = args[0];
    datMark(getInfo(value)->data);
    return NULL;
}

/* Documented in header. */
METH_IMPL(DerivedData, get) {
    zvalue value = args[0];
    zvalue key = args[1];

    zvalue data = getInfo(value)->data;
    return (data == NULL) ? NULL : get(data, key);
}

/* Documented in header. */
METH_IMPL(DerivedData, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valEqNullOk(getInfo(value)->data, getInfo(other)->data)
        ? value : NULL;
}

/* Documented in header. */
METH_IMPL(DerivedData, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valOrderNullOk(getInfo(value)->data, getInfo(other)->data);
}

/** Initializes the module. */
MOD_INIT(DerivedData) {
    MOD_USE(Data);

    // Note: The `typeSystem` module initializes `TYPE_DerivedData`.

    METH_BIND(DerivedData, gcMark);
    METH_BIND(DerivedData, get);
    METH_BIND(DerivedData, totalEq);
    METH_BIND(DerivedData, totalOrder);
}

/* Documented in header. */
zvalue TYPE_DerivedData = NULL;
