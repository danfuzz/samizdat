/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Type` data type
 */

#ifndef _TYPE_TYPE_H_
#define _TYPE_TYPE_H_

#include "dat.h"

#include <stdbool.h>


/** Type value for in-model type `Type`. */
extern zvalue TYPE_Type;

/**
 * Asserts that the given value has the given type. If not, this aborts
 * the process with a diagnostic message. **Note:** This does not do a
 * validity check on the given arguments.
 */
void assertHasType(zvalue value, zvalue type);

/**
 * Returns the unique index for the type of the given value.
 */
zint get_typeIndex(zvalue value);

/**
 * Returns true iff the type of the given value (that is, `get_type(value)`)
 * is either the given type or is a subtype of the given type.
 */
bool hasType(zvalue value, zvalue type);

/**
 * Returns true iff the types of the given values (that is, `get_type()` on
 * each) are the same.
 */
bool haveSameType(zvalue value, zvalue other);

/**
 * Makes a new core type. `name` is the type's name. `parent` is its
 * super-type. `selfish` indicates whether the type should  be considered
 * "selfish." Values of a selfish type have unique identity values which
 * can be retrieved using `valSelfIdOf`. These values are automatically
 * used when comparing values of the same type. It is a fatal error to call
 * this function more than once with any given name.
 */
zvalue makeCoreType(zvalue name, zvalue parent, bool selfish);

/**
 * Returns the type value for the derived data type with the given name.
 */
zvalue makeDerivedDataType(zvalue name);

/**
 * Returns `true` if the given `type` has the indicated `secret`. `secret`
 * may be passed as `NULL`.
 */
bool typeHasSecret(zvalue type, zvalue secret);

/**
 * Returns the unique index for the given type.
 */
zint typeIndex(zvalue type);

/**
 * Returns true iff the given type is derived (whether pure data or not).
 */
bool typeIsDerived(zvalue type);

/**
 * Returns true iff the given type is "selfish." That is, this returns
 * true if values of the type can be fruitfully used as the argument
 * to `valSelfIdOf`.
 */
bool typeIsSelfish(zvalue type);

/**
 * Gets the parent type of the given type.
 */
zvalue typeParent(zvalue type);

#endif
