// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Object.h"
#include "type/SymbolTable.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Payload data for all object values.
 */
typedef struct {
    /** Data payload. */
    zvalue data;
} ObjectInfo;

/**
 * Gets the info of an object.
 */
static ObjectInfo *getInfo(zvalue obj) {
    return (ObjectInfo *) datPayload(obj);
}

/**
 * Method to get the given object's data payload. This is the function
 * that's bound as the instance method for the `secret` symbol. It's not
 * bound directly on `Object` so as to provide data encapsulation.
 */
METH_IMPL_0(Object, privateDataOf) {
    return getInfo(ths)->data;
}


//
// Exported Definitions
//

// Documented in header.
zvalue makeObject(zvalue cls, zvalue secret, zvalue data) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        assertValid(secret);
        assertValidOrNull(data);
    }

    if (!classHasParent(cls, CLS_Object)) {
        die("Attempt to call `makeObject` on an improper class.");
    }

    if (!classHasSecret(cls, secret)) {
        die("Mismatched `secret` on call to `makeObject`.");
    }

    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
    } else {
        assertHasClass(data, CLS_SymbolTable);
    }

    zvalue result = datAllocValue(cls, sizeof(ObjectInfo));
    ObjectInfo *info = getInfo(result);
    info->data = data;

    return result;
}

// Documented in header.
zvalue makeObjectClass(zvalue name, zvalue secret,
        zvalue classMethods, zvalue instanceMethods) {
    if (classMethods == NULL) {
        classMethods = EMPTY_SYMBOL_TABLE;
    }

    if (instanceMethods == NULL) {
        instanceMethods = EMPTY_SYMBOL_TABLE;
    }

    instanceMethods = METH_CALL(cat,
        instanceMethods,
        symbolTableFromArgs(
            secret, FUNC_VALUE(Object_privateDataOf),
            NULL));

    return makeClass(name, CLS_Object, secret,
        classMethods, instanceMethods);
}


//
// Class Definition
//

// Documented in header.
FUNC_IMPL_2_3(Object_makeObject, cls, secret, data) {
    return makeObject(cls, secret, data);
}

// Documented in header.
FUNC_IMPL_2_4(Object_makeObjectClass, name, secret,
        classMethods, instanceMethods) {
    return makeObjectClass(name, secret, classMethods, instanceMethods);
}

// Documented in header.
METH_IMPL_0(Object, gcMark) {
    ObjectInfo *info = getInfo(ths);

    datMark(info->data);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Object) {
    MOD_USE(Value);

    CLS_Object = makeCoreClass("Object", CLS_Value,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Object, gcMark),
            NULL));

    FUN_Object_makeObject = datImmortalize(FUNC_VALUE(Object_makeObject));
    FUN_Object_makeObjectClass =
        datImmortalize(FUNC_VALUE(Object_makeObjectClass));
}

// Documented in header.
zvalue CLS_Object = NULL;

// Documented in header.
zvalue FUN_Object_makeObject = NULL;

// Documented in header.
zvalue FUN_Object_makeObjectClass = NULL;
