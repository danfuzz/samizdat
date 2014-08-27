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
SYM_DECL(abs);

/** Method `.add(number)`: Documented in spec. */
SYM_DECL(add);

/** Method `.div(number)`: Documented in spec. */
SYM_DECL(div);

/** Method `.divEu(number)`: Documented in spec. */
SYM_DECL(divEu);

/** Method `.mod(number)`: Documented in spec. */
SYM_DECL(mod);

/** Method `.modEu(number)`: Documented in spec. */
SYM_DECL(modEu);

/** Method `.mul(number)`: Documented in spec. */
SYM_DECL(mul);

/** Method `.neg()`: Documented in spec. */
SYM_DECL(neg);

/** Method `.sign()`: Documented in spec. */
SYM_DECL(sign);

/** Method `.sub(number)`: Documented in spec. */
SYM_DECL(sub);

#endif
