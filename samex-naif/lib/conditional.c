// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "type/Map.h"
#include "util.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(ifIs) {
    if (FUN_CALL(args[0]) != NULL) {
        return FUN_CALL(args[1]);
    } else if (argCount == 3) {
        return FUN_CALL(args[2]);
    } else {
        return NULL;
    }
}

// Documented in spec.
FUN_IMPL_DECL(ifNot) {
    if (FUN_CALL(args[0]) == NULL) {
        return FUN_CALL(args[1]);
    } else {
        return NULL;
    }
}

// Documented in spec.
FUN_IMPL_DECL(ifSwitch) {
    zvalue testFunction = args[0];
    zvalue valueFunctions = args[1];
    zvalue defaultFunction = (argCount >= 3) ? args[2] : NULL;

    zvalue value = FUN_CALL(testFunction);

    if (value == NULL) {
        die("Void result from `ifSwitch` call to `testFunction`.");
    }

    zvalue consequentFunction = cm_get(valueFunctions, value);

    if (consequentFunction != NULL) {
        return FUN_CALL(consequentFunction, value);
    }

    return (defaultFunction == NULL)
        ? NULL
        : FUN_CALL(defaultFunction, value);
}

// Documented in spec.
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

// Documented in spec.
FUN_IMPL_DECL(ifValueAnd) {
    zvalue results[argCount];

    for (zint i = 0; i < argCount; i++) {
        results[i] = methCall(args[i], SYM(call), i, results);

        if (results[i] == NULL) {
            return NULL;
        }
    }

    return results[argCount - 1];
}

// Documented in spec.
FUN_IMPL_DECL(ifValueAndElse) {
    zint funcCount = argCount - 2;
    zvalue thenFunc = args[funcCount];
    zvalue elseFunc = args[funcCount + 1];
    zvalue results[funcCount];

    for (zint i = 0; i < funcCount; i++) {
        results[i] = methCall(args[i], SYM(call), i, results);

        if (results[i] == NULL) {
            return methCall(elseFunc, SYM(call), 0, NULL);
        }
    }

    return methCall(thenFunc, SYM(call), funcCount, results);
}

// Documented in spec.
FUN_IMPL_DECL(ifValueOr) {
    for (zint i = 0; i < argCount; i++) {
        zvalue result = FUN_CALL(args[i]);
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

// Documented in spec.
FUN_IMPL_DECL(loop) {
    zvalue function = args[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        FUN_CALL(function);
        datFrameReturn(save, NULL);
    }
}

// Documented in spec.
FUN_IMPL_DECL(maybeValue) {
    zvalue function = args[0];
    zvalue value = FUN_CALL(function);

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}
