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

/**
 * C version of class method `Record.new()`. `data` is allowed to be `NULL`,
 * which is treated as `@{}`.
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
