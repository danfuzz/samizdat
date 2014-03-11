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
#include "type/OneOff.h"

#include <stdbool.h>


/** Type value for in-model type `Type`. */
extern zvalue TYPE_Type;

/**
 * Asserts that all the given arguments have the same type. If not, this
 * aborts the process with a diagnostic message. **Note:** This does not
 * do a validity check on the values.
 */
void assertAllHaveSameType(zint argCount, const zvalue *args);

/**
 * Asserts that the given value has the given type. If not, this aborts
 * the process with a diagnostic message. If given a non-`Type` value for
 * `type`, this takes it to name a transparent derived type. **Note:** This
 * does not do a validity check on the given arguments.
 */
void assertHasType(zvalue value, zvalue type);

/**
 * Gets a new core type, given its name. When given the same name twice, this
 * returns identical results. `identified` indicates whether the type should
 * be considered "identified." Values of an "identified" type have unique
 * identity values which can be retrieved using `valIdentityOf`. These values
 * are automatically used when comparing values of the same type.
 */
zvalue coreTypeFromName(zvalue name, bool identified);

/**
 * Returns true iff the type of the given value (that is, `typeOf(value)`)
 * is as given.
 */
bool hasType(zvalue value, zvalue typeOrName);

/**
 * Returns true iff the types of the given values (that is, `typeOf()` on
 * each) are the same.
 */
bool haveSameType(zvalue v1, zvalue v2);

/**
 * Returns the type value for the transparent type with the given name.
 */
zvalue typeFromName(zvalue name);

/**
 * Returns `true` if the given `type` has the indicated `secret`. `secret`
 * may be passed as `NULL`.
 */
bool typeHasSecret(zvalue type, zvalue secret);

/**
 * Returns the unique index for the given type (or transparent type name).
 */
zint typeIndex(zvalue typeOrName);

/**
 * Returns the unique index for the type of the given value.
 */
zint typeIndexOf(zvalue value);

/**
 * Returns true iff the given type is a derived type (whether opaque or
 * transparent).
 */
bool typeIsDerived(zvalue type);

/**
 * Returns true iff the given type is "identified." That is, this returns
 * true if values of the type can be fruitfully used as the argument
 * to `valIdentityOf`.
 */
bool typeIsIdentified(zvalue type);

/**
 * Returns true iff the given type is a transparent derived type.
 */
bool typeIsTransparentDerived(zvalue type);

/**
 * Gets the type of the given value. `value` must be a valid value (in
 * particular, non-`NULL`). The return value is of type `Type`.
 */
zvalue typeOf(zvalue value);

/**
 * Gets the parent type of the given type.
 */
zvalue typeParent(zvalue type);

#endif
