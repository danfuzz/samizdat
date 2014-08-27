// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Object` class
//

#ifndef _TYPE_OBJECT_H_
#define _TYPE_OBJECT_H_

#include "type/Value.h"


/** Class value for in-model class `Object`. */
extern zvalue CLS_Object;

/** Method `.objectDataOf(secret)`: Documented in spec. */
SYM_DECL(objectDataOf);

/** Global function `makeObject`: Documented in spec. */
extern zvalue FUN_Object_makeObject;

/** Global function `makeObjectClass`: Documented in spec. */
extern zvalue FUN_Object_makeObjectClass;

/**
 * Returns an object value with the given class and with the given
 * optional data payload (`NULL` indicating a class-only value). `cls` must
 * be an object class, and `secret` must match `cls`'s secret. The result is
 * a value of the indicated `cls`.
 */
zvalue makeObject(zvalue cls, zvalue secret, zvalue data);

/**
 * Makes a new object class with the given name and secret.
 */
zvalue makeObjectClass(zvalue name, zvalue secret);

/**
 * Calls the method `objectDataOf()`.
 */
zvalue objectDataOf(zvalue obj, zvalue secret);

#endif
