/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletPutNth) {
    requireExactly(argCount, 3);
    return datListletPutNth(args[0], datIntFromIntlet(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletInsNth) {
    requireExactly(argCount, 3);
    return datListletInsNth(args[0], datIntFromIntlet(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAdd) {
    requireExactly(argCount, 2);
    return datListletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletNth) {
    requireRange(argCount, 2, 3);

    if (!datTypeIs(args[1], DAT_INTLET)) {
        return NULL;
    }

    return datListletNth(args[0], datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletDelNth) {
    requireExactly(argCount, 2);
    return datListletDelNth(args[0], datIntFromIntlet(args[1]));
}
