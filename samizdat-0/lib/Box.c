/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Box.h"

#include <stddef.h>


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMutableBox) {
    zvalue value = (argCount == 1) ? args[0] : NULL;
    return makeMutableBox(value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeYieldBox) {
    return makeYieldBox();
}
