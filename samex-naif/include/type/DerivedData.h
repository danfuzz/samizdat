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

/** Global function `dataOf`: Documented in spec. */
extern zvalue FUN_DerivedData_dataOf;

/** Global function `makeData`: Documented in spec. */
extern zvalue FUN_DerivedData_makeData;

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
 * derived data value. This returns `NULL` if `value` is either not a derived
 * data value or does not have a payload.
 */
zvalue dataOf(zvalue value);

#endif
