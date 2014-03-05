/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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

/* Documented in spec. */
FUN_IMPL_DECL(ifIs) {
    if (FUN_CALL(args[0]) != NULL) {
        return FUN_CALL(args[1]);
    } else if (argCount == 3) {
        return FUN_CALL(args[2]);
    } else {
        return NULL;
    }
}

/* Documented in spec. */
FUN_IMPL_DECL(ifNot) {
    if (FUN_CALL(args[0]) == NULL) {
        return FUN_CALL(args[1]);
    } else {
        return NULL;
    }
}

/* Documented in spec. */
FUN_IMPL_DECL(ifValue) {
    zvalue result = FUN_CALL(args[0]);

    if (result != NULL) {
        return FUN_CALL(args[1], result);
    } else if (argCount == 3) {
        return FUN_CALL(args[2]);
    } else {
        return NULL;
    }
}

/* Documented in spec. */
FUN_IMPL_DECL(ifValueOr) {
    zvalue result = FUN_CALL(args[0]);

    if (result != NULL) {
        return result;
    } else {
        return FUN_CALL(args[1]);
    }
}

/* Documented in spec. */
FUN_IMPL_DECL(ifValues) {
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

/* Documented in spec. */
FUN_IMPL_DECL(loop) {
    zvalue function = args[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        FUN_CALL(function);
        datFrameReturn(save, NULL);
    }
}

/* Documented in spec. */
FUN_IMPL_DECL(optValue) {
    zvalue function = args[0];
    zvalue value = FUN_CALL(function);

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}
