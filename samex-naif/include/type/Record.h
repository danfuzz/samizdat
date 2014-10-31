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
 * Gets the values of two keys out of a record, storing them via the given
 * pointers. Returns `true` iff all keys were bound.
 */
bool recGet2(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2);

/**
 * Gets the values of three keys out of a record, storing them via the given
 * pointers. Returns `true` iff all keys were bound.
 */
bool recGet3(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2,
        zvalue key3, zvalue *got3);

/**
 * Returns whether the given `record` has the given `name`.
 */
bool recHasName(zvalue record, zvalue name);

/**
 * Get the symbol index of the given `record`'s name.
 */
zint recNameIndex(zvalue record);

#endif
