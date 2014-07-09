// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

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


//
// Private Definitions
//

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


//
// Exported Definitions
//

// Documented in header.
zvalue dataOf(zvalue value) {
    if (!typeIsDerived(get_type(value))) {
        return NULL;
    }

    zvalue result = getInfo(value)->data;
    datFrameAdd(result);  // Because `value` might immediately become garbage.
    return result;
}

// Documented in header.
zvalue makeData(zvalue type, zvalue data, zvalue secret) {
    assertValidOrNull(data);
    assertValidOrNull(secret);

    if (!typeHasSecret(type, secret)) {
        die("Attempt to create derived value with incorrect secret.");
    }

    zvalue result = datAllocValue(type, sizeof(DerivedDataInfo));
    ((DerivedDataInfo *) datPayload(result))->data = data;

    return result;
}


//
// Type Definition
//

/** Function (not method) `dataOf`. Documented in spec. */
METH_IMPL(DerivedData, dataOf) {
    zvalue value = args[0];

    return dataOf(value);
}

// Documented in header.
METH_IMPL(DerivedData, gcMark) {
    zvalue value = args[0];
    datMark(getInfo(value)->data);
    return NULL;
}

// Documented in header.
METH_IMPL(DerivedData, get) {
    zvalue value = args[0];
    zvalue key = args[1];

    if (!typeHasSecret(get_type(value), NULL)) {
        return NULL;
    }

    zvalue data = getInfo(value)->data;
    return (data == NULL) ? NULL : get(data, key);
}

/** Function (not method) `makeData`. Documented in spec. */
METH_IMPL(DerivedData, makeData) {
    zvalue type = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    return makeData(type, value, NULL);
}

// Documented in header.
METH_IMPL(DerivedData, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valEqNullOk(getInfo(value)->data, getInfo(other)->data)
        ? value : NULL;
}

// Documented in header.
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

    FUN_DerivedData_dataOf = makeBuiltin(1, 1,
        METH_NAME(DerivedData, dataOf), 0,
        stringFromUtf8(-1, "DerivedData.dataOf"));
    datImmortalize(FUN_DerivedData_dataOf);

    FUN_DerivedData_makeData = makeBuiltin(1, 2,
        METH_NAME(DerivedData, makeData), 0,
        stringFromUtf8(-1, "DerivedData.makeData"));
    datImmortalize(FUN_DerivedData_makeData);
}

// Documented in header.
zvalue TYPE_DerivedData = NULL;

// Documented in header.
zvalue FUN_DerivedData_dataOf = NULL;

// Documented in header.
zvalue FUN_DerivedData_makeData = NULL;
