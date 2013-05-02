/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(argsMap) {
    requireAtLeast(argCount, 1);

    zvalue function = args[0];

    args++;
    argCount--;

    zvalue result[argCount];
    zint at = 0;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = langCall(function, 1, &args[i]);
        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return datListletFromArray(at, result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(argsReduce) {
    requireAtLeast(argCount, 2);

    zvalue function = args[0];
    zvalue funArgs[2];

    funArgs[0] = args[1];

    args += 2;
    argCount -= 2;

    for (zint i = 0; i < argCount; i++) {
        funArgs[1] = args[i];

        zvalue result = langCall(function, 2, funArgs);

        if (result != NULL) {
            funArgs[0] = result;
        }
    }

    return funArgs[0];
}

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
PRIM_IMPL(loop) {
    requireExactly(argCount, 1);

    zvalue function = args[0];
    for (;;) {
        langCall(function, 0, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loopReduce) {
    requireExactly(argCount, 2);

    zvalue result = args[0];
    zvalue function = args[1];

    for (;;) {
        zvalue next = langCall(function, 1, &result);
        if (next != NULL) {
            result = next;
        }
    }
}
