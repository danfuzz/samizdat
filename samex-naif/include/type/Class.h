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
void assertHasClass(zvalue value, zvalue type);

/**
 * Returns the unique index for the type of the given value.
 */
zint get_typeIndex(zvalue value);

/**
 * Returns true iff the type of the given value (that is, `get_type(value)`)
 * is either the given type or is a subtype of the given type.
 */
bool hasClass(zvalue value, zvalue type);

/**
 * Returns true iff the types of the given values (that is, `get_type()` on
 * each) are the same.
 */
bool haveSameClass(zvalue value, zvalue other);

/**
 * Makes a new core type. `name` is the type's name. `parent` is its
 * super-type. It is a fatal error to call this function more than once with
 * any given name.
 */
zvalue makeCoreType(zvalue name, zvalue parent);

/**
 * Returns the type value for the derived data type with the given name.
 */
zvalue makeDerivedDataType(zvalue name);

/**
 * Returns the unique index for the given type.
 */
zint typeIndex(zvalue type);

/**
 * Returns true iff the given type is derived (whether pure data or not).
 */
bool typeIsDerived(zvalue type);

/**
 * Gets the name of the given type.
 */
zvalue typeName(zvalue type);

/**
 * Gets the parent type of the given type.
 */
zvalue typeParent(zvalue type);

#endif
