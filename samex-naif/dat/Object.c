// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Builtin.h"
#include "type/Object.h"
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

    zvalue result = datAllocValue(cls, sizeof(ObjectInfo));
    ObjectInfo *info = getInfo(result);
    info->secret = secret;
    info->data = data;

    return result;
}

// Documented in header.
zvalue makeObjectClass(zvalue name, zvalue secret) {
    assertHasClass(name, CLS_Selector);
    return makeClass(name, CLS_Object, secret, NULL, NULL);
}

//
// Class Definition
//

// Documented in header.
METH_IMPL(Object, objectDataOf) {
    zvalue obj = args[0];
    zvalue secret = args[1];

    ObjectInfo *info = getInfo(obj);

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
METH_IMPL(Object, gcMark) {
    zvalue obj = args[0];
    ObjectInfo *info = getInfo(obj);

    datMark(info->secret);
    datMark(info->data);
    return NULL;
}

/** Function (not method) `makeObject`. Documented in spec. */
METH_IMPL(Object, makeObject) {
    zvalue cls = args[0];
    zvalue secret = args[1];
    zvalue data = (argCount == 3) ? args[2] : NULL;

    return makeObject(cls, secret, data);
}

/** Function (not method) `makeObjectClass`. Documented in spec. */
METH_IMPL(Object, makeObjectClass) {
    zvalue name = args[0];
    zvalue secret = args[1];

    return makeObjectClass(name, secret);
}

/** Initializes the module. */
MOD_INIT(Object) {
    MOD_USE(Value);

    SEL_INIT(objectDataOf);

    // Note: The `objectModel` module initializes `CLS_Object`.
    classBindMethods(CLS_Object,
        NULL,
        symbolTableFromArgs(
            SEL_METH(Object, objectDataOf),
            SEL_METH(Object, gcMark),
            NULL));

    FUN_Object_makeObject = makeBuiltin(2, 3,
        METH_NAME(Object, makeObject), 0,
        stringFromUtf8(-1, "Object.makeObject"));
    datImmortalize(FUN_Object_makeObject);

    FUN_Object_makeObjectClass = makeBuiltin(2, 2,
        METH_NAME(Object, makeObjectClass), 0,
        stringFromUtf8(-1, "Object.makeObjectClass"));
    datImmortalize(FUN_Object_makeObjectClass);
}

// Documented in header.
zvalue CLS_Object = NULL;

// Documented in header.
SEL_DEF(objectDataOf);

// Documented in header.
zvalue FUN_Object_makeObject = NULL;

// Documented in header.
zvalue FUN_Object_makeObjectClass = NULL;
