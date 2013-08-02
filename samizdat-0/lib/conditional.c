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

    if (datCall(args[0], 0, NULL) != NULL) {
        return datCall(args[1], 0, NULL);
    } else if (argCount == 3) {
        return datCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifNot) {
    requireExactly(argCount, 2);

    if (datCall(args[0], 0, NULL) == NULL) {
        return datCall(args[1], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValue) {
    requireRange(argCount, 2, 3);

    zvalue result = datCall(args[0], 0, NULL);

    if (result != NULL) {
        return datCall(args[1], 1, &result);
    } else if (argCount == 3) {
        return datCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValueOr) {
    requireExactly(argCount, 2);

    zvalue result = datCall(args[0], 0, NULL);

    if (result != NULL) {
        return result;
    } else {
        return datCall(args[1], 0, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loop) {
    requireExactly(argCount, 1);

    zvalue function = args[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        datCall(function, 0, NULL);
        datFrameReturn(save, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loopReduce) {
    requireAtLeast(argCount, 1);

    zstackPointer save = datFrameStart();
    zvalue function = args[0];
    zvalue innerArgs = datListFromArray(argCount - 1, &args[1]);

    for (;;) {
        zvalue nextArgs = datApply(function, innerArgs);
        if (nextArgs != NULL) {
            innerArgs = nextArgs;
            datFrameReset(save, innerArgs);
        }
    }
}
