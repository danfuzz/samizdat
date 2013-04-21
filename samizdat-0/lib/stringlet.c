/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromChar) {
    requireExactly(argCount, 1);
    return datStringletFromIntlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromChars) {
    requireExactly(argCount, 1);
    return datStringletFromListlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(charsFromStringlet) {
    requireExactly(argCount, 1);
    return datListletFromStringlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletAdd) {
    requireExactly(argCount, 2);
    return datStringletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletNth) {
    requireExactly(argCount, 2);

    zvalue stringlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datIntletFromInt(datStringletGet(stringlet, index));
}
