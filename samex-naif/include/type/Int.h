// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Int` class
//

#ifndef _TYPE_INT_H_
#define _TYPE_INT_H_

#include "type/Core.h"


/** Class value for in-model class `Int`. */
extern zvalue CLS_Int;

/** The standard value `0`. */
extern zvalue INT_0;

/** The standard value `1`. */
extern zvalue INT_1;

/** The standard value `-1`. */
extern zvalue INT_NEG1;

/**
 * Gets an int value equal to the given `zint`. In this
 * implementation, ints are restricted to only taking on the range
 * of 64-bit signed twos-complement integers.
 */
zvalue intFromZint(zint value);

/**
 * Gets a `zint` equal to the given int value. `intval` must be an
 * int. It is an error if the value is out of range.
 */
zint zintFromInt(zvalue intval);

#endif
