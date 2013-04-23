/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromIntlet) {
    requireExactly(argCount, 1);
    return datStringletFromIntlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletAdd) {
    requireExactly(argCount, 2);
    return datStringletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletNth) {
    requireRange(argCount, 2, 3);

    zvalue result = NULL;
    if (datTypeIs(args[1], DAT_INTLET)) {
        zint ch = datStringletNth(args[0], datIntFromIntlet(args[1]));
        if (ch >= 0) {
            result = datIntletFromInt(ch);
        }
    }

    if (result == NULL) {
        return (argCount == 3) ? args[3] : NULL;
    } else {
        return result;
    }
}
