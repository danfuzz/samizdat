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

/** Array of all derived data classes, in sort order (possibly stale). */
static zvalue theClasses[DAT_MAX_CLASSES];

/** Count of derived data classes. */
static zint theClassCount = 0;

/** Whether `theClasses` needs a sort. */
static bool theNeedSort = true;


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

/**
 * Compares two classes by name. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue cls1 = *(zvalue *) vptr1;
    zvalue cls2 = *(zvalue *) vptr2;

    return valZorder(className(cls1), className(cls2));
}

/**
 * Compares a name with a class. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    zvalue name1 = (zvalue) key;
    zvalue cls2 = *(zvalue *) vptr;

    return valZorder(name1, className(cls2));
}

/**
 * Finds an existing class with the given name, if any.
 */
static zvalue findClass(zvalue name) {
    if (theNeedSort) {
        mergesort(theClasses, theClassCount, sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue *found = (zvalue *) bsearch(
        name, theClasses, theClassCount, sizeof(zvalue), searchOrder);

    return (found == NULL) ? NULL : *found;
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
    zvalue result = findClass(name);

    if (result != NULL) {
        return result;
    }

    if (theClassCount == DAT_MAX_CLASSES) {
        die("Too many derived data classes!");
    }

    result = makeClass(name, CLS_DerivedData, NULL, NULL, NULL);
    theClasses[theClassCount] = result;
    theClassCount++;
    theNeedSort = true;

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
