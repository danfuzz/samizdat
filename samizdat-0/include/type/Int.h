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
#include "type/Collection.h"


/** Type value for in-model type `Int`. */
extern zvalue TYPE_Int;

/** Generic `abs(int)`: Documented in spec. */
extern zvalue GFN_abs;

/** Generic `add(int, int)`: Documented in spec. */
extern zvalue GFN_add;

/** Generic `and(int, int)`: Documented in spec. */
extern zvalue GFN_and;

/** Generic `bit(int, int)`: Documented in spec. */
extern zvalue GFN_bit;

/** Generic `bit(int, int)`: Documented in spec. */
extern zvalue GFN_bitSize;

/** Generic `div(int, int)`: Documented in spec. */
extern zvalue GFN_div;

/** Generic `divEu(int, int)`: Documented in spec. */
extern zvalue GFN_divEu;

/** Generic `mod(int, int)`: Documented in spec. */
extern zvalue GFN_mod;

/** Generic `modEu(int, int)`: Documented in spec. */
extern zvalue GFN_modEu;

/** Generic `mul(int, int)`: Documented in spec. */
extern zvalue GFN_mul;

/** Generic `neg(int)`: Documented in spec. */
extern zvalue GFN_neg;

/** Generic `not(int)`: Documented in spec. */
extern zvalue GFN_not;

/** Generic `or(int, int)`: Documented in spec. */
extern zvalue GFN_or;

/** Generic `shl(int, int)`: Documented in spec. */
extern zvalue GFN_shl;

/** Generic `shr(int, int)`: Documented in spec. */
extern zvalue GFN_shr;

/** Generic `sign(int)`: Documented in spec. */
extern zvalue GFN_sign;

/** Generic `sub(int, int)`: Documented in spec. */
extern zvalue GFN_sub;

/** Generic `xor(int, int)`: Documented in spec. */
extern zvalue GFN_xor;

/** The standard value `0`. */
extern zvalue INT_0;

/** The standard value `1`. */
extern zvalue INT_1;

/** The standard value `-1`. */
extern zvalue INT_NEG1;

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
