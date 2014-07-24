// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Type` data type
//

#ifndef _TYPE_TYPE_H_
#define _TYPE_TYPE_H_

#include <stdbool.h>

#include "type/Value.h"


/** Type value for in-model type `Type`. */
extern zvalue TYPE_Type;

/**
 * Asserts that the given value has the given type. If not, this aborts
 * the process with a diagnostic message. **Note:** This does not do a
 * validity check on the given arguments.
 */
void assertHasClass(zvalue value, zvalue cls);

/**
 * Returns the unique index for the given type.
 */
zint classIndex(zvalue cls);

/**
 * Returns true iff the given type is derived (whether pure data or not).
 */
bool classIsDerived(zvalue cls);

/**
 * Gets the name of the given type.
 */
zvalue className(zvalue cls);

/**
 * Gets the parent type of the given type.
 */
zvalue classParent(zvalue cls);

/**
 * Returns the unique index for the type of the given value.
 */
zint get_classIndex(zvalue value);

/**
 * Returns true iff the type of the given value (that is, `get_class(value)`)
 * is either the given type or is a subtype of the given type.
 */
bool hasClass(zvalue value, zvalue cls);

/**
 * Returns true iff the types of the given values (that is, `get_class()` on
 * each) are the same.
 */
bool haveSameClass(zvalue value, zvalue other);

/**
 * Makes a new core type. `name` is the type's name. `parent` is its
 * super-type. It is a fatal error to call this function more than once with
 * any given name.
 */
zvalue makeCoreClass(zvalue name, zvalue parent);

/**
 * Returns the type value for the derived data type with the given name.
 */
zvalue makeDerivedDataClass(zvalue name);

#endif
