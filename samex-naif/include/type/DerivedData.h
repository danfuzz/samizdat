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
 * Returns a derived data value with the given type tag and with the given
 * optional data payload (`NULL` indicating a type-only value). `type` must
 * be a derived data type. The result is a value of the indicated `type`.
 */
zvalue makeData(zvalue type, zvalue data);

/**
 * Gets the data payload of the given derived data value. This returns `NULL`
 * if `value` doesn't have a payload. This dies with an error if `value` is
 * not a derived data value.
 */
zvalue dataOf(zvalue value);

#endif
