/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(tokenType) {
    requireExactly(argCount, 1);
    return datDerivType(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(tokenValue) {
    requireExactly(argCount, 1);
    return datDerivData(args[0]);
}
