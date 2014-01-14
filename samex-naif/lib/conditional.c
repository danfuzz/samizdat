/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/Function.h"
#include "type/List.h"


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifIs) {
    if (FUN_CALL(args[0]) != NULL) {
        return FUN_CALL(args[1]);
    } else if (argCount == 3) {
        return FUN_CALL(args[2]);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifNot) {
    if (FUN_CALL(args[0]) == NULL) {
        return FUN_CALL(args[1]);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValue) {
    zvalue result = FUN_CALL(args[0]);

    if (result != NULL) {
        return FUN_CALL(args[1], result);
    } else if (argCount == 3) {
        return FUN_CALL(args[2]);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValueOr) {
    zvalue result = FUN_CALL(args[0]);

    if (result != NULL) {
        return result;
    } else {
        return FUN_CALL(args[1]);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValues) {
    zvalue testFunctions = args[0];
    zvalue valueFunction = args[1];
    zvalue voidFunction = (argCount == 3) ? args[2] : NULL;
    zint size = collSize(testFunctions);
    zvalue testArr[size];
    zvalue results[size];

    arrayFromList(testArr, testFunctions);

    for (zint i = 0; i < size; i++) {
        zvalue one = results[i] = funCall(testArr[i], i, results);
        if (one == NULL) {
            return voidFunction ? FUN_CALL(voidFunction) : NULL;
        }
    }

    return funCall(valueFunction, size, results);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loop) {
    zvalue function = args[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        FUN_CALL(function);
        datFrameReturn(save, NULL);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(loopReduce) {
    zstackPointer save = datFrameStart();
    zvalue function = args[0];
    zvalue innerArgs = listFromArray(argCount - 1, &args[1]);

    for (;;) {
        zvalue nextArgs = funApply(function, innerArgs);
        if (nextArgs != NULL) {
            innerArgs = nextArgs;
            datFrameReset(save, innerArgs);
        }
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(optValue) {
    zvalue function = args[0];
    zvalue value = FUN_CALL(function);

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}
