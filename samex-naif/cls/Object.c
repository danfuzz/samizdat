// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
 * Helper for the two constructor methods, which does all the work.
 */
static zvalue doNew(zvalue cls, zvalue data) {
    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
    } else {
        assertHasClass(data, CLS_SymbolTable);
    }

    zvalue result = datAllocValue(cls, sizeof(ObjectInfo));

    getInfo(result)->data = data;
    return result;
}

/**
 * Class method to construct an instance. This is the function that's bound as
 * the class method for the `new` symbol.
 */
CMETH_IMPL_0_opt(Object, new, data) {
    return doNew(thsClass, data);
}

/**
 * Instance method to construct an instance. This is the function that's bound
 * as the instance method for the `new` symbol.
 */
METH_IMPL_0_opt(Object, new, data) {
    return doNew(classOf(ths), data);
}

/**
 * Method to get the given object's data payload. This is the function
 * that's bound as the instance method for the `access` symbol.
 */
METH_IMPL_0_opt(Object, access, key) {
    zvalue data = getInfo(ths)->data;
    return (key == NULL) ? data : symtabGet(data, key);
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_2_opt_opt(Object, subclass, name, config,
        classMethods, instanceMethods) {
    if (thsClass != CLS_Object) {
        die("Invalid parent class: %s", cm_debugString(thsClass));
    }

    if (classMethods == NULL) {
        classMethods = EMPTY_SYMBOL_TABLE;
    }

    if (instanceMethods == NULL) {
        instanceMethods = EMPTY_SYMBOL_TABLE;
    }

    zvalue accessSecret = cm_get(config, SYM(access));
    zvalue newSecret = cm_get(config, SYM(new));

    if (accessSecret != NULL) {
        instanceMethods = cm_cat(instanceMethods,
            METH_TABLE(accessSecret, FUNC_VALUE(Object_access)));
    }

    if (newSecret != NULL) {
        classMethods = cm_cat(classMethods,
            METH_TABLE(newSecret, FUNC_VALUE(class_Object_new)));
        instanceMethods = cm_cat(instanceMethods,
            METH_TABLE(newSecret, FUNC_VALUE(Object_new)));
    }

    return makeClass(name, CLS_Object, classMethods, instanceMethods);
}

// Documented in header.
METH_IMPL_0(Object, gcMark) {
    ObjectInfo *info = getInfo(ths);

    datMark(info->data);
    return NULL;
}

// Documented in spec.
METH_IMPL_1(Object, crossEq, other) {
    if (ths == other) {
        return ths;
    }

    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`crossEq` called with incompatible arguments.");
    }

    return METH_CALL(getInfo(ths)->data, crossEq, getInfo(other)->data);
}

// Documented in spec.
METH_IMPL_1(Object, crossOrder, other) {
    if (ths == other) {
        return SYM(same);
    }

    // Note: `other` not guaranteed to have the same class as `ths`.
    if (!haveSameClass(ths, other)) {
        die("`crossOrder` called with incompatible arguments.");
    }

    return METH_CALL(getInfo(ths)->data, crossOrder, getInfo(other)->data);
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
            METH_BIND(Object, crossEq),
            METH_BIND(Object, crossOrder),
            METH_BIND(Object, gcMark)));
}

// Documented in header.
zvalue CLS_Object = NULL;
