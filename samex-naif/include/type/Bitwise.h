// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Bitwise` protocol
//

#ifndef _TYPE_BITWISE_H_
#define _TYPE_BITWISE_H_

#include "type/Value.h"


/** Method `.and(bitwise)`: Documented in spec. */
SYM_DECL(and);

/** Method `.bit(int)`: Documented in spec. */
SYM_DECL(bit);

/** Method `.bitSize()`: Documented in spec. */
SYM_DECL(bitSize);

/** Method `.not()`: Documented in spec. */
SYM_DECL(not);

/** Method `.or(bitwise)`: Documented in spec. */
SYM_DECL(or);

/** Method `.shl(int)`: Documented in spec. */
SYM_DECL(shl);

/** Method `.shr(int)`: Documented in spec. */
SYM_DECL(shr);

/** Method `.xor(bitwise)`: Documented in spec. */
SYM_DECL(xor);

#endif
