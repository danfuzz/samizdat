/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifTrue) {
    requireRange(argCount, 2, 3);

    if (constBoolFromBoolean(langCall(args[0], 0, NULL))) {
        return langCall(args[1], 0, NULL);
    } else if (argCount == 3) {
        return langCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValue) {
    requireRange(argCount, 2, 3);

    zvalue result = langCall(args[0], 0, NULL);

    if (result != NULL) {
        return langCall(args[1], 1, &result);
    } else if (argCount == 3) {
        return langCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(while) {
    requireExactly(argCount, 1);

    zvalue test = args[0];
    while (constBoolFromBoolean(langCall(test, 0, NULL))) {
        // No loop body necessary.
    }

    return NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(whileReduce) {
    requireExactly(argCount, 2);

    zvalue result = args[0];
    zvalue func = args[1];

    for (;;) {
        zvalue next = langCall(func, 1, &result);
        if (next == NULL) {
            return result;
        }

        result = next;
    }
}
