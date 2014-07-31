// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Collection.h"
#include "type/DerivedData.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
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
    return METH_CALL(dataOf, value);
}

// Documented in header.
zvalue makeData(zvalue cls, zvalue data) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        assertValidOrNull(data);
    }

    if (!classIsDerived(cls)) {
        die("Attempt to call `makeData` on an improper class.");
    }

    zvalue result = datAllocValue(cls, sizeof(DerivedDataInfo));
    ((DerivedDataInfo *) datPayload(result))->data = data;

    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(DerivedData, dataOf) {
    zvalue value = args[0];

    // The `datFrameAdd()` call is because `value` might immediately become
    // garbage.
    return datFrameAdd(getInfo(value)->data);
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

    zvalue data = getInfo(value)->data;
    return (data == NULL) ? NULL : get(data, key);
}

/** Function (not method) `makeData`. Documented in spec. */
METH_IMPL(DerivedData, makeData) {
    zvalue cls = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    return makeData(cls, value);
}

// Documented in header.
METH_IMPL(DerivedData, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be of same class.

    if (!haveSameClass(value, other)) {
        die("`totalEq` called with incompatible arguments.");
    }

    return valEqNullOk(getInfo(value)->data, getInfo(other)->data)
        ? value : NULL;
}

// Documented in header.
METH_IMPL(DerivedData, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be of same class.

    if (!haveSameClass(value, other)) {
        die("`totalOrder` called with incompatible arguments.");
    }

    return valOrderNullOk(getInfo(value)->data, getInfo(other)->data);
}

/** Initializes the module. */
MOD_INIT(DerivedData) {
    MOD_USE(Data);

    // Note: The `objectModel` module initializes `CLS_DerivedData`.

    GFN_dataOf = makeGeneric(1, 1, stringFromUtf8(-1, "dataOf"));
    datImmortalize(GFN_dataOf);

    METH_BIND(DerivedData, dataOf);
    METH_BIND(DerivedData, gcMark);
    METH_BIND(DerivedData, get);
    METH_BIND(DerivedData, totalEq);
    METH_BIND(DerivedData, totalOrder);

    FUN_DerivedData_makeData = makeBuiltin(1, 2,
        METH_NAME(DerivedData, makeData), 0,
        stringFromUtf8(-1, "DerivedData.makeData"));
    datImmortalize(FUN_DerivedData_makeData);
}

// Documented in header.
zvalue CLS_DerivedData = NULL;

// Documented in header.
zvalue GFN_dataOf = NULL;

// Documented in header.
zvalue FUN_DerivedData_makeData = NULL;
