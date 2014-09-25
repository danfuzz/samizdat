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

/** Global function `makeRecordClass`: Documented in spec. */
extern zvalue FUN_Record_makeRecordClass;

/**
 * Calls the method `dataOf()`.
 */
zvalue dataOf(zvalue value);

/**
 * Returns a record with the given class and with the given optional data
 * payload (`NULL` indicating an empty payload). `cls` must be a record class.
 * The result is a value of the indicated `cls`. */
zvalue makeRecord(zvalue cls, zvalue data);

/**
 * Returns the class value for the record class with the given name.
 */
zvalue makeRecordClass(zvalue name);

/**
 * Get the symbol index of the given `record`'s name.
 */
zint recNameIndex(zvalue record);

#endif
