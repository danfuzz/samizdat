/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * One-Off generics
 */

#ifndef _TYPE_ONE_OFF_H_
#define _TYPE_ONE_OFF_H_

#include "dat.h"

/** Generic `cat(value, more*)`: Documented in spec. */
extern zvalue GFN_cat;

/** Generic `get(value, key)`: Documented in spec. */
extern zvalue GFN_get;

/** Generic `keyOf(value)`: Documented in spec. */
extern zvalue GFN_keyOf;

/** Generic `nameOf(value)`: Documented in spec. */
extern zvalue GFN_nameOf;

/** Generic `sizeOf(collection)`: Documented in spec. */
extern zvalue GFN_sizeOf;

/** Generic `toInt(value)`: Documented in spec. */
extern zvalue GFN_toInt;

/** Generic `toNumber(value)`: Documented in spec. */
extern zvalue GFN_toNumber;

/** Generic `toString(value)`: Documented in spec. */
extern zvalue GFN_toString;

/** Generic `valueOf(value)`: Documented in spec. */
extern zvalue GFN_valueOf;

/**
 * Calls the `get` generic.
 */
zvalue collGet(zvalue coll, zvalue key);

/**
 * Calls `sizeOf` on the given collection, converting the result to a `zint`.
 */
zint collSize(zvalue coll);

/**
 * Calls `nameOf` on the given value.
 */
zvalue nameOf(zvalue value);

/**
 * Calls `toString` on the given value, returning the result as a `char *`.
 * The caller is responsible for `free()`ing the result. As a convenience,
 * this converts `NULL` into `"(null)"`.
 */
char *valToString(zvalue value);

#endif
