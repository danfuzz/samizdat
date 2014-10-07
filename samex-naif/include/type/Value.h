// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Value` base class
//
// **Note:** As a slight concession to conciseness, the name prefix used for
// this class is `val` and not `value`.
//

#ifndef _TYPE_VALUE_H_
#define _TYPE_VALUE_H_

#include <stdbool.h>
#include <stddef.h>

#include "type/declare.h"
#include "type/methodCalls.h"
#include "type/symbols.h"


/** Class value for in-model class `Value`. */
extern zvalue CLS_Value;

/**
 * Compares two values for equality, returning the first argument to
 * represent logical-true or `NULL` for logical-false. This calls through
 * to `totalEq` given values of the same class. **Note:** It is invalid to
 * pass `NULL` to this function.
 */
zvalue valEq(zvalue value, zvalue other);

/**
 * Like `valEq`, except that `NULL`s are accepted as arguments, and the
 * function returns a `bool` (of necessity, since a `zvalue` result would be
 * ambiguous).
 */
bool valEqNullOk(zvalue value, zvalue other);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ -1, 0, 1 }`, with the usual comparison result meaning.
 * This calls through to `totalOrder` given values of the same class. **Note:**
 * It is invalid to pass `NULL` to this function.
 */
zvalue valOrder(zvalue value, zvalue other);

#endif
