/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Primitive implementations (exported via the context)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(die) {
    requireRange(argCount, 0, 1);

    const char *message =
        (argCount == 0) ? "Alas" : datStringletEncodeUtf8(args[0], NULL);

    die("%s", message);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(readFile) {
    requireExactly(argCount, 1);
    return readFile(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(writeFile) {
    requireExactly(argCount, 2);
    writeFile(args[0], args[1]);
    return NULL;
}
