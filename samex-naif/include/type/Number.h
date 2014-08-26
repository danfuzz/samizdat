// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Number` protocol
//

#ifndef _TYPE_NUMBER_H_
#define _TYPE_NUMBER_H_

#include "type/Value.h"


/** Method `.abs()`: Documented in spec. */
SYM_DECL(1, 1, abs);

/** Method `.add(number)`: Documented in spec. */
SYM_DECL(2, 2, add);

/** Method `.div(number)`: Documented in spec. */
SYM_DECL(2, 2, div);

/** Method `.divEu(number)`: Documented in spec. */
SYM_DECL(2, 2, divEu);

/** Method `.mod(number)`: Documented in spec. */
SYM_DECL(2, 2, mod);

/** Method `.modEu(number)`: Documented in spec. */
SYM_DECL(2, 2, modEu);

/** Method `.mul(number)`: Documented in spec. */
SYM_DECL(2, 2, mul);

/** Method `.neg()`: Documented in spec. */
SYM_DECL(1, 1, neg);

/** Method `.sign()`: Documented in spec. */
SYM_DECL(1, 1, sign);

/** Method `.sub(number)`: Documented in spec. */
SYM_DECL(2, 2, sub);

#endif
