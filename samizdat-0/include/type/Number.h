/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Number` protocol
 */

#ifndef _TYPE_NUMBER_H_
#define _TYPE_NUMBER_H_

#include "pb.h"


/** Generic `abs(number)`: Documented in spec. */
extern zvalue GFN_abs;

/** Generic `add(number, number)`: Documented in spec. */
extern zvalue GFN_add;

/** Generic `div(number, number)`: Documented in spec. */
extern zvalue GFN_div;

/** Generic `divEu(number, number)`: Documented in spec. */
extern zvalue GFN_divEu;

/** Generic `mod(number, number)`: Documented in spec. */
extern zvalue GFN_mod;

/** Generic `modEu(number, number)`: Documented in spec. */
extern zvalue GFN_modEu;

/** Generic `mul(number, number)`: Documented in spec. */
extern zvalue GFN_mul;

/** Generic `neg(number)`: Documented in spec. */
extern zvalue GFN_neg;

/** Generic `sign(number)`: Documented in spec. */
extern zvalue GFN_sign;

/** Generic `sub(number, number)`: Documented in spec. */
extern zvalue GFN_sub;

#endif
