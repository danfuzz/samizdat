/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ineg) {
    requireExactly(argCount, 1);
    return datIntletFromInt(-datIntFromIntlet(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(iadd) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) + datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(isub) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) - datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(imul) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) * datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(idiv) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) / datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(imod) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) % datIntFromIntlet(args[1]));
}
