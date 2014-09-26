// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Record` class
//

#ifndef _TYPE_RECORD_H_
#define _TYPE_RECORD_H_

#include "type/Data.h"


/** Class value for in-model class `Record`. */
extern zvalue CLS_Record;

/** Method `.dataOf()`: Documented in spec. */
SYM_DECL(dataOf);

/** Global function `makeRecord`: Documented in spec. */
extern zvalue FUN_Record_makeRecord;

/**
 * Calls the method `dataOf()`.
 */
zvalue dataOf(zvalue value);

/**
 * Returns a record with the given class or name, and with the given optional
 * data payload (`NULL` indicating an empty payload). `cls` must be a record
 * class or a symbol. The result is a record of the indicated / implied `cls`.
 */
zvalue makeRecord(zvalue clsOrName, zvalue data);

/**
 * Returns whether the given `record` has the given `name`.
 */
bool recHasName(zvalue record, zvalue name);

/**
 * Get the symbol index of the given `record`'s name.
 */
zint recNameIndex(zvalue record);

#endif
