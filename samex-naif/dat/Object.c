// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Builtin.h"
#include "type/Object.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Payload data for all object values.
 */
typedef struct {
    /** Class secret; copied from the class. */
    zvalue secret;

    /** Data payload. */
    zvalue data;
} ObjectInfo;

/**
 * Gets the info of an object.
 */
static ObjectInfo *getInfo(zvalue obj) {
    return (ObjectInfo *) datPayload(obj);
}


//
// Exported Definitions
//

// Documented in header.
zvalue objectDataOf(zvalue obj, zvalue secret) {
    return METH_CALL(objectDataOf, obj, secret);
}

// Documented in header.
zvalue makeObject(zvalue cls, zvalue secret, zvalue data) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        assertValid(secret);
        assertValidOrNull(data);
    }

    if (!classEq(classParent(cls), CLS_Object)) {
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
    info->secret = secret;
    info->data = data;

    return result;
}

// Documented in header.
zvalue makeObjectClass(zvalue name, zvalue secret) {
    assertHasClass(name, CLS_Symbol);
    return makeClass(name, CLS_Object, secret, NULL, NULL);
}

//
// Class Definition
//

// Documented in header.
FUNC_IMPL_2_3(Object_makeObject, cls, secret, data) {
    return makeObject(cls, secret, data);
}

// Documented in header.
FUNC_IMPL_2(Object_makeObjectClass, name, secret) {
    return makeObjectClass(name, secret);
}

// Documented in header.
METH_IMPL_1(Object, objectDataOf, secret) {
    ObjectInfo *info = getInfo(ths);

    // Note: It's important to pass `info->secret` first, so that it's the
    // one whose `totalEq` method is used. The given `secret` can't be
    // trusted to behave.
    if (!valEq(info->secret, secret)) {
        die("Mismatched secret on call to `objectDataOf`.");
    }

    // The `datFrameAdd()` call is because `obj` might immediately become
    // garbage.
    return datFrameAdd(info->data);
}

// Documented in header.
METH_IMPL_0(Object, gcMark) {
    ObjectInfo *info = getInfo(ths);

    datMark(info->secret);
    datMark(info->data);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Object) {
    MOD_USE(Value);

    SYM_INIT(objectDataOf);

    CLS_Object = makeCoreClass("Object", CLS_Value,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Object, objectDataOf),
            METH_BIND(Object, gcMark),
            NULL));

    FUN_Object_makeObject = datImmortalize(FUNC_VALUE(Object_makeObject));
    FUN_Object_makeObjectClass =
        datImmortalize(FUNC_VALUE(Object_makeObjectClass));
}

// Documented in header.
zvalue CLS_Object = NULL;

// Documented in header.
SYM_DEF(objectDataOf);

// Documented in header.
zvalue FUN_Object_makeObject = NULL;

// Documented in header.
zvalue FUN_Object_makeObjectClass = NULL;
