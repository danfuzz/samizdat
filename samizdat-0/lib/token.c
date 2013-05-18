/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(tokenHasValue) {
    requireExactly(argCount, 1);
    return constBooleanFromBool(datTokenValue(args[0]) != NULL);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletType) {
    requireExactly(argCount, 1);
    return datTokenType(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(tokenValue) {
    requireRange(argCount, 1, 2);

    zvalue result = datTokenValue(args[0]);

    if ((result == NULL) && (argCount == 2)) {
        return args[1];
    }

    return result;
}
