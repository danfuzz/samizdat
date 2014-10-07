// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Record` class
//

#ifndef _TYPE_RECORD_H_
#define _TYPE_RECORD_H_

#include "type/Core.h"


/** Class value for in-model class `Record`. */
extern zvalue CLS_Record;

/** Global function `makeRecord()`: Documented in spec. */
extern zvalue FUN_Record_makeRecord;

/**
 * Calls the method `dataOf()`.
 */
zvalue dataOf(zvalue value);

/**
 * C version of library function `makeRecord()`. Documented in spec.
 */
zvalue makeRecord(zvalue name, zvalue data);

/**
 * Returns whether the given `record` has the given `name`.
 */
bool recHasName(zvalue record, zvalue name);

/**
 * Get the symbol index of the given `record`'s name.
 */
zint recNameIndex(zvalue record);

#endif
