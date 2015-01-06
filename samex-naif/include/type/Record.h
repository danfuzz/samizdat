// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
 * Makes a record from a `name` (a symbol) and a `zarray` where the contents
 * are a list of "splayed" key-then-value pairs. The keys must all be symbols
 * (of course). The `zarray` arrangement is identical to that of
 * `symtabFromZarray()`.
 */
zvalue recFromZarray(zvalue name, zarray arr);

/**
 * Gets the values of one key out of a record, storing it via the given
 * pointer. Returns `true` iff the key was bound.
 */
bool recGet1(zvalue record, zvalue key, zvalue *got);

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
 * Gets the values of four keys out of a record, storing them via the given
 * pointers. Returns `true` iff all keys were bound.
 */
bool recGet4(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2,
        zvalue key3, zvalue *got3,
        zvalue key4, zvalue *got4);

/**
 * Returns whether the given `record` has the given `name`.
 */
bool recHasName(zvalue record, zvalue name);

/**
 * Get the symbol index of the given `record`'s name.
 */
zint recNameIndex(zvalue record);

#endif
