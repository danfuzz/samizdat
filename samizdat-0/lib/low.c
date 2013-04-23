/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "util.h"


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrder) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrderIs) {
    requireRange(argCount, 3, 4);

    zorder comp = datOrder(args[0], args[1]);
    bool result =
        (comp == datIntFromIntlet(args[2])) ||
        ((argCount == 4) && (comp == datIntFromIntlet(args[3])));

    return langBooleanFromBool(result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowSize) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowType) {
    requireExactly(argCount, 1);
    return langLowTypeName(args[0]);
}
