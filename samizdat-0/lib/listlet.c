/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAppend) {
    requireExactly(argCount, 2);
    return datListletAppend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletPrepend) {
    requireExactly(argCount, 2);
    return datListletPrepend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAdd) {
    requireExactly(argCount, 2);
    return datListletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletNth) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datListletNth(listlet, index);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletDelNth) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datListletDelete(listlet, index);
}
