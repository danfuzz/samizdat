// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `DerivedData` data type
//

#ifndef _TYPE_DERIVED_DATA_H_
#define _TYPE_DERIVED_DATA_H_

#include "type/Data.h"


/** Type value for in-model type `DerivedData`. */
extern zvalue TYPE_DerivedData;

/**
 * Returns a derived value with the given type tag, and with the given
 * optional data payload (`NULL` indicating a type-only value). `type` must
 * be a value of type `Type`, and the result is a value of the indicated type.
 * If `type` represents an opaque type, then `secret` must match the secret
 * known by `type`. If `type` is a derived data type, then `secret` must be
 * `NULL`.
 */
zvalue makeData(zvalue type, zvalue data, zvalue secret);

/**
 * Gets the data payload of the given value, if it is a value-bearing
 * derived data value. `value` must be a valid value (in particular,
 * non-`NULL`). This is a convenient shorthand for calling
 * `valDataOf(value, NULL)`.
 */
zvalue dataOf(zvalue value);

/**
 * Gets the data payload of the given value, if possible. This behaves
 * as follows:
 *
 * * If `value` is a core value, this returns `NULL`.
 *
 * * If `value`'s type secret does not match the given secret, this returns
 *   `NULL`. Notably, if `value` is of a derived data type and `secret`
 *   is *not* passed as `NULL`, this returns `NULL`.
 *
 * * If `value` does not have any payload data, this returns `NULL`.
 *
 * * Otherwise, this returns the payload data of `value`.
 */
zvalue valDataOf(zvalue value, zvalue secret);

#endif
