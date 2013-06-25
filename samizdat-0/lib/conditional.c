/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifIs) {
    requireRange(argCount, 2, 3);

    if (langCall(args[0], 0, NULL) != NULL) {
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
PRIM_IMPL(loop) {
    requireExactly(argCount, 1);

    zvalue function = args[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        langCall(function, 0, NULL);
        datFrameReturn(save, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loopReduce) {
    requireExactly(argCount, 2);

    zstackPointer save = datFrameStart();
    zvalue result = args[0];
    zvalue function = args[1];

    for (;;) {
        zvalue next = langCall(function, 1, &result);
        if (next != NULL) {
            result = next;
            datFrameReset(save, result);
        }
    }
}
