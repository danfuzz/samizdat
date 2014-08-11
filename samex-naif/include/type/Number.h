// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Number` protocol
//

#ifndef _TYPE_NUMBER_H_
#define _TYPE_NUMBER_H_

#include "type/Value.h"


/** Method `abs(number)`: Documented in spec. */
SEL_DECL(1, 1, abs);

/** Method `add(number, number)`: Documented in spec. */
SEL_DECL(2, 2, add);

/** Method `div(number, number)`: Documented in spec. */
SEL_DECL(2, 2, div);

/** Method `divEu(number, number)`: Documented in spec. */
SEL_DECL(2, 2, divEu);

/** Method `mod(number, number)`: Documented in spec. */
SEL_DECL(2, 2, mod);

/** Method `modEu(number, number)`: Documented in spec. */
SEL_DECL(2, 2, modEu);

/** Method `mul(number, number)`: Documented in spec. */
SEL_DECL(2, 2, mul);

/** Method `neg(number)`: Documented in spec. */
SEL_DECL(1, 1, neg);

/** Method `sign(number)`: Documented in spec. */
SEL_DECL(1, 1, sign);

/** Method `sub(number, number)`: Documented in spec. */
SEL_DECL(2, 2, sub);

#endif
