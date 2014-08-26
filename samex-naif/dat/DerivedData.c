// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/Collection.h"
#include "type/DerivedData.h"
#include "type/Int.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Array mapping selector indices to derived data classes. */
static zvalue theClasses[DAT_MAX_SELECTORS];


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

// Documented in header.
zvalue makeDerivedDataClass(zvalue name) {
    // `selectorIndex` will bail if `name` isn't a selector.
    zint index = selectorIndex(name);
    zvalue result = theClasses[index];

    if (result != NULL) {
        return result;
    }

    result = makeClass(name, CLS_DerivedData, NULL, NULL, NULL);
    theClasses[index] = result;

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

/** Function (not method) `makeDerivedDataClass`. Documented in spec. */
METH_IMPL(DerivedData, makeDerivedDataClass) {
    return makeDerivedDataClass(args[0]);
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

    SEL_INIT(dataOf);

    // Note: The `objectModel` module initializes `CLS_DerivedData`.
    classBindMethods(CLS_DerivedData,
        NULL,
        selectorTableFromArgs(
            SEL_METH(DerivedData, dataOf),
            SEL_METH(DerivedData, gcMark),
            SEL_METH(DerivedData, get),
            SEL_METH(DerivedData, totalEq),
            SEL_METH(DerivedData, totalOrder),
            NULL));

    FUN_DerivedData_makeData = makeBuiltin(1, 2,
        METH_NAME(DerivedData, makeData), 0,
        stringFromUtf8(-1, "DerivedData.makeData"));
    datImmortalize(FUN_DerivedData_makeData);

    FUN_DerivedData_makeDerivedDataClass = makeBuiltin(1, 1,
        METH_NAME(DerivedData, makeDerivedDataClass), 0,
        stringFromUtf8(-1, "DerivedData.makeDerivedDataClass"));
    datImmortalize(FUN_DerivedData_makeDerivedDataClass);
}

// Documented in header.
zvalue CLS_DerivedData = NULL;

// Documented in header.
SEL_DEF(dataOf);

// Documented in header.
zvalue FUN_DerivedData_makeData = NULL;

// Documented in header.
zvalue FUN_DerivedData_makeDerivedDataClass = NULL;
