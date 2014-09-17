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
 * Returns a derived data value with the given class tag and with the given
 * optional data payload (`NULL` indicating a class-only value). `cls` must
 * be a derived data class. The result is a value of the indicated `cls`.
 */
zvalue makeRecord(zvalue cls, zvalue data);

/**
 * Calls the method `dataOf()`.
 */
zvalue dataOf(zvalue value);

/**
 * Returns the class value for the derived data class with the given name.
 */
zvalue makeRecordClass(zvalue name);

#endif
