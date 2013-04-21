/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Primitive implementations (exported via the context)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletAdd) {
    requireExactly(argCount, 2);
    return datMapletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletKeys) {
    requireExactly(argCount, 1);
    return datMapletKeys(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletGet) {
    requireRange(argCount, 2, 3);

    zvalue result = datMapletGet(args[0], args[1]);

    if (result == NULL) {
        if (argCount != 3) {
            die("Key not found in maplet.");
        } else {
            return args[3];
        }
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletPut) {
    requireExactly(argCount, 3);
    return datMapletPut(args[0], args[1], args[2]);
}
