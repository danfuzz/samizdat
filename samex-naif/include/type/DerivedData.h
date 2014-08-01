// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `DerivedData` class
//

#ifndef _TYPE_DERIVED_DATA_H_
#define _TYPE_DERIVED_DATA_H_

#include "type/Data.h"


/** Class value for in-model class `DerivedData`. */
extern zvalue CLS_DerivedData;

/** Generic `dataOf(derivedData)`: Documented in spec. */
SEL_DECL(dataOf);

/** Global function `makeData`: Documented in spec. */
extern zvalue FUN_DerivedData_makeData;

/**
 * Returns a derived data value with the given class tag and with the given
 * optional data payload (`NULL` indicating a class-only value). `cls` must
 * be a derived data class. The result is a value of the indicated `cls`.
 */
zvalue makeData(zvalue cls, zvalue data);

/**
 * Calls generic `dataOf()`.
 */
zvalue dataOf(zvalue value);

#endif
