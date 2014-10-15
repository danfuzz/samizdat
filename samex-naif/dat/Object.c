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
 * Method to construct an instance. This is the function that's bound as the
 * class method for the `secret` symbol.
 */
CMETH_IMPL_0_1(Object, new, data) {
    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
    } else {
        assertHasClass(data, CLS_SymbolTable);
    }

    zvalue result = datAllocValue(thsClass, sizeof(ObjectInfo));

    getInfo(result)->data = data;
    return result;
}

/**
 * Method to get the given object's data payload. This is the function
 * that's bound as the instance method for the `secret` symbol.
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


//
// Class Definition
//

// Documented in spec.
FUNC_IMPL_2_3(Object_makeObject, cls, secret, data) {
    return makeObject(cls, secret, data);
}

// Documented in spec.
CMETH_IMPL_2_4(Object, subclass, name, secret,
        classMethods, instanceMethods) {
    if (thsClass != CLS_Object) {
        die("Invalid parent class: %s", cm_debugString(thsClass));
    }

    zvalue extraInstanceMethods = METH_TABLE(
        secret, FUNC_VALUE(Object_privateDataOf));
    instanceMethods = (instanceMethods == NULL)
        ? extraInstanceMethods
        : cm_cat(instanceMethods, extraInstanceMethods);

    zvalue extraClassMethods = METH_TABLE(
        secret, FUNC_VALUE(class_Object_new));
    classMethods = (classMethods == NULL)
        ? extraClassMethods
        : cm_cat(classMethods, extraClassMethods);

    return makeClass(name, CLS_Object, secret, classMethods, instanceMethods);
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

    // Note: This does *not* inherit from `Core`, as this class is the
    // base for all non-core classes.
    CLS_Object = makeCoreClass(SYM(Object), CLS_Value,
        METH_TABLE(
            CMETH_BIND(Object, subclass)),
        METH_TABLE(
            METH_BIND(Object, gcMark)));

    FUN_Object_makeObject = datImmortalize(FUNC_VALUE(Object_makeObject));
}

// Documented in header.
zvalue CLS_Object = NULL;

// Documented in header.
zvalue FUN_Object_makeObject = NULL;
