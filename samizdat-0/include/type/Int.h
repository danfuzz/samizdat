/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Int` data type
 */

#ifndef _TYPE_INT_H_
#define _TYPE_INT_H_

#include "pb.h"


/** Type value for in-model type `Int`. */
extern zvalue TYPE_Int;

/** Generic `abs(int)`: Documented in spec. */
extern zvalue GFN_abs;

/** Generic `neg(int)`: Documented in spec. */
extern zvalue GFN_neg;

/** Generic `not(int)`: Documented in spec. */
extern zvalue GFN_not;

/** Generic `sign(int)`: Documented in spec. */
extern zvalue GFN_sign;

/** The standard value `0`. */
extern zvalue PB_0;

/** The standard value `1`. */
extern zvalue PB_1;

/** The standard value `-1`. */
extern zvalue PB_NEG1;

/**
 * Gets the `zchar` of the given int, asserting that the value
 * is in fact an int and in range for same.
 */
zchar zcharFromInt(zvalue intval);

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
