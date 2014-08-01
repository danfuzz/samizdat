// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Number` protocol
//

#ifndef _TYPE_NUMBER_H_
#define _TYPE_NUMBER_H_

#include "type/Value.h"


/** Generic `abs(number)`: Documented in spec. */
SEL_DECL(1, 1, abs);

/** Generic `add(number, number)`: Documented in spec. */
SEL_DECL(2, 2, add);

/** Generic `div(number, number)`: Documented in spec. */
SEL_DECL(2, 2, div);

/** Generic `divEu(number, number)`: Documented in spec. */
SEL_DECL(2, 2, divEu);

/** Generic `mod(number, number)`: Documented in spec. */
SEL_DECL(2, 2, mod);

/** Generic `modEu(number, number)`: Documented in spec. */
SEL_DECL(2, 2, modEu);

/** Generic `mul(number, number)`: Documented in spec. */
SEL_DECL(2, 2, mul);

/** Generic `neg(number)`: Documented in spec. */
SEL_DECL(1, 1, neg);

/** Generic `sign(number)`: Documented in spec. */
SEL_DECL(1, 1, sign);

/** Generic `sub(number, number)`: Documented in spec. */
SEL_DECL(2, 2, sub);

#endif
