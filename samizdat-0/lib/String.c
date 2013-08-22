/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Int.h"
#include "type/String.h"
#include "util.h"


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(charFromInt) {
    return stringFromZchar(zcharFromInt(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intFromChar) {
    zvalue string = args[0];

    return intFromZint(zcharFromString(string));
}
