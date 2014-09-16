// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Collection.h"
#include "type/DerivedData.h"
#include "type/Int.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Array mapping symbol indices to derived data classes. */
static zvalue theClasses[DAT_MAX_SYMBOLS];


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
    if (!classIsDerived(cls)) {
        die("Attempt to call `makeData` on an improper class.");
    }

    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
    } else {
        assertHasClass(data, CLS_SymbolTable);
    }

    zvalue result = datAllocValue(cls, sizeof(DerivedDataInfo));
    ((DerivedDataInfo *) datPayload(result))->data = data;

    return result;
}

// Documented in header.
zvalue makeDerivedDataClass(zvalue name) {
    // `symbolIndex` will bail if `name` isn't a symbol.
    zint index = symbolIndex(name);
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
FUNC_IMPL_1_2(DerivedData_makeData, cls, value) {
    return makeData(cls, value);
}

// Documented in header.
FUNC_IMPL_1(DerivedData_makeDerivedDataClass, name) {
    return makeDerivedDataClass(name);
}

// Documented in header.
METH_IMPL_0(DerivedData, dataOf) {
    // The `datFrameAdd()` call is because `value` might immediately become
    // garbage.
    return datFrameAdd(getInfo(ths)->data);
}

// Documented in header.
METH_IMPL_0(DerivedData, gcMark) {
    datMark(getInfo(ths)->data);
    return NULL;
}

// Documented in header.
METH_IMPL_1(DerivedData, get, key) {
    zvalue data = getInfo(ths)->data;
    return (data == NULL) ? NULL : get(data, key);
}

// Documented in header.
METH_IMPL_1(DerivedData, totalEq, other) {
    // Note: `other` not guaranteed to be of same class.

    if (!haveSameClass(ths, other)) {
        die("`totalEq` called with incompatible arguments.");
    }

    return valEqNullOk(getInfo(ths)->data, getInfo(other)->data)
        ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(DerivedData, totalOrder, other) {
    // Note: `other` not guaranteed to be of same class.

    if (!haveSameClass(ths, other)) {
        die("`totalOrder` called with incompatible arguments.");
    }

    return valOrderNullOk(getInfo(ths)->data, getInfo(other)->data);
}

/** Initializes the module. */
MOD_INIT(DerivedData) {
    MOD_USE(Data);

    SYM_INIT(dataOf);

    CLS_DerivedData = makeCoreClass("DerivedData", CLS_Data,
        NULL,
        symbolTableFromArgs(
            METH_BIND(DerivedData, dataOf),
            METH_BIND(DerivedData, gcMark),
            METH_BIND(DerivedData, get),
            METH_BIND(DerivedData, totalEq),
            METH_BIND(DerivedData, totalOrder),
            NULL));

    FUN_DerivedData_makeData =
        datImmortalize(FUNC_VALUE(DerivedData_makeData));

    FUN_DerivedData_makeDerivedDataClass =
        datImmortalize(FUNC_VALUE(DerivedData_makeDerivedDataClass));
}

// Documented in header.
zvalue CLS_DerivedData = NULL;

// Documented in header.
SYM_DEF(dataOf);

// Documented in header.
zvalue FUN_DerivedData_makeData = NULL;

// Documented in header.
zvalue FUN_DerivedData_makeDerivedDataClass = NULL;
