/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifIs) {
    if (fnCall(args[0], 0, NULL) != NULL) {
        return fnCall(args[1], 0, NULL);
    } else if (argCount == 3) {
        return fnCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifNot) {
    if (fnCall(args[0], 0, NULL) == NULL) {
        return fnCall(args[1], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValue) {
    zvalue result = fnCall(args[0], 0, NULL);

    if (result != NULL) {
        return fnCall(args[1], 1, &result);
    } else if (argCount == 3) {
        return fnCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValueOr) {
    zvalue result = fnCall(args[0], 0, NULL);

    if (result != NULL) {
        return result;
    } else {
        return fnCall(args[1], 0, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loop) {
    zvalue function = args[0];
    for (;;) {
        zstackPointer save = pbFrameStart();
        fnCall(function, 0, NULL);
        pbFrameReturn(save, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loopReduce) {
    zstackPointer save = pbFrameStart();
    zvalue function = args[0];
    zvalue innerArgs = listFromArray(argCount - 1, &args[1]);

    for (;;) {
        zvalue nextArgs = fnApply(function, innerArgs);
        if (nextArgs != NULL) {
            innerArgs = nextArgs;
            pbFrameReset(save, innerArgs);
        }
    }
}
