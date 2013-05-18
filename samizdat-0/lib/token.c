/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletHasValue) {
    requireExactly(argCount, 1);
    return constBooleanFromBool(datHighletValue(args[0]) != NULL);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletType) {
    requireExactly(argCount, 1);
    return datHighletType(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletValue) {
    requireRange(argCount, 1, 2);

    zvalue result = datHighletValue(args[0]);

    if ((result == NULL) && (argCount == 2)) {
        return args[1];
    }

    return result;
}
