// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Bitwise` protocol
//

#ifndef _TYPE_BITWISE_H_
#define _TYPE_BITWISE_H_

#include "type/Value.h"


/** Generic `and(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, and);

/** Generic `bit(bitwise, int)`: Documented in spec. */
SEL_DECL(2, 2, bit);

/** Generic `bitSize(bitwise)`: Documented in spec. */
SEL_DECL(1, 1, bitSize);

/** Generic `not(bitwise)`: Documented in spec. */
SEL_DECL(1, 1, not);

/** Generic `or(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, or);

/** Generic `shl(bitwise, int)`: Documented in spec. */
SEL_DECL(2, 2, shl);

/** Generic `xor(bitwise, bitwise)`: Documented in spec. */
SEL_DECL(2, 2, xor);

#endif
