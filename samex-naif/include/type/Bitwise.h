// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Bitwise` protocol
//

#ifndef _TYPE_BITWISE_H_
#define _TYPE_BITWISE_H_

#include "type/Value.h"


/** Method `and(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, and);

/** Method `bit(bitwise, int)`: Documented in spec. */
SEL_DECL(2, 2, bit);

/** Method `bitSize(bitwise)`: Documented in spec. */
SEL_DECL(1, 1, bitSize);

/** Method `not(bitwise)`: Documented in spec. */
SEL_DECL(1, 1, not);

/** Method `or(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, or);

/** Method `shl(bitwise, int)`: Documented in spec. */
SEL_DECL(2, 2, shl);

/** Method `xor(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, xor);

#endif
