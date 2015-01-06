// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Cmp` class
//

#ifndef _TYPE_CMP_H_
#define _TYPE_CMP_H_

#include "type/Core.h"


/** Class value for in-model class `Cmp`. */
extern zvalue CLS_Cmp;

/**
 * Performs the equivalent of `Cmp.eq(value, other)`. **Note:** It is invalid
 * to pass `NULL` to this function.
 */
zvalue cmpEq(zvalue value, zvalue other);

/**
 * Like `cmpEq`, except that `NULL`s are accepted as arguments, and the
 * function returns a `bool` (of necessity, since a `zvalue` result would be
 * ambiguous).
 */
bool cmpEqNullOk(zvalue value, zvalue other);

/**
 * Performs the equivalent of `Cmp.order()`. **Note:** It is invalid to pass
 * `NULL` to this function.
 */
zvalue cmpOrder(zvalue value, zvalue other);

#endif
