// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// One-Off methods
//

#ifndef _TYPE_ONE_OFF_H_
#define _TYPE_ONE_OFF_H_

#include "type/Value.h"

/**
 * Calls the `get` method.
 */
zvalue get(zvalue coll, zvalue key);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value.
 */
zvalue nth(zvalue value, zint index);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value, and
 * converting a non-void return value &mdash; which must be a single-character
 * `String` &mdash; to a `zint` in the range of a `zchar`. A void return
 * value gets converted to `-1`.
 */
zint nthChar(zvalue value, zint index);

/**
 * Calls `get_size` on the given value, converting the result to a `zint`.
 */
zint get_size(zvalue value);

#endif
