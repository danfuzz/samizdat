/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Type` data type
 */

#ifndef _TYPE_TYPE_H_
#define _TYPE_TYPE_H_

#include "pb.h"


/** Type value for in-model type `Type`. */
extern zvalue TYPE_Type;

/**
 * Asserts that the given value is a valid `zvalue`, furthermore has the
 * given type. If not, this aborts the process with a diagnostic message.
 * If given a non-`Type` value for `type`, this takes it to name a
 * transparent derived type.
 */
void assertHasType(zvalue value, zvalue type);

/**
 * Asserts that the given two values are valid `zvalue`s, and furthermore
 * that they have the same type. If not, this aborts the process
 * with a diagnostic message.
 */
void assertHaveSameType(zvalue v1, zvalue v2);

/**
 * Gets a new core type, given its name. When given the same name twice, this
 * returns identical results. `identified` indicates whether the type should
 * be considered "identified". Values of an "identified" type have unique
 * identity values which can be retrieved using `valIdentityOf`. These values
 * are automatically used when comparing values of the same type.
 */
zvalue coreTypeFromName(zvalue name, bool identified);

/**
 * Returns true iff the type of the given value (that is, `typeOf(value)`)
 * is as given.
 */
bool hasType(zvalue value, zvalue type);

/**
 * Returns true iff the types of the given values (that is, `typeOf()` on
 * each) are the same.
 */
bool haveSameType(zvalue v1, zvalue v2);

/**
 * Returns true iff the given type is "identified". That is, this returns
 * true if values of the type can be fruitfully used as the argument
 * to `valIdentityOf`.
 */
bool typeIsIdentified(zvalue type);

/**
 * Gets the name of the given type. If given a non-`Type` value for `type`,
 * this takes it to name a transparent derived type; as such it will return
 * `type` itself in these cases.
 */
zvalue typeName(zvalue type);

/**
 * Gets the overt data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`). For transparent derived types,
 * this returns the name of the type, and not a `Type` value per se.
 */
zvalue typeOf(zvalue value);

/**
 * Gets the parent type of the given type. If given a non-`Type` value for
 * `type`, this takes it to name a transparent derived type; as such it
 * will return `TYPE_Value` in these cases.
 */
zvalue typeParent(zvalue type);

#endif
