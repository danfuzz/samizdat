// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/If.h"
#include "type/List.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_rest(If, and, functions) {
    if (functions.size == 0) {
        return NULL;
    }

    zvalue results[functions.size];

    for (zint i = 0; i < functions.size; i++) {
        results[i] =
            methCall(functions.elems[i], SYM(call), (zarray) {i, results});

        if (results[i] == NULL) {
            return NULL;
        }
    }

    return results[functions.size - 1];
}

// Documented in spec.
CMETH_IMPL_rest_2(If, andThenElse, functions, thenFunction, elseFunction) {
    zvalue results[functions.size];

    for (zint i = 0; i < functions.size; i++) {
        results[i] =
            methCall(functions.elems[i], SYM(call), (zarray) {i, results});

        if (results[i] == NULL) {
            return methCall(elseFunction, SYM(call), EMPTY_ZARRAY);
        }
    }

    return
        methCall(thenFunction, SYM(call), (zarray) {functions.size, results});
}

// Documented in spec.
CMETH_IMPL_2_opt(If, cases, testFunction, valueFunctions, defaultFunction) {
    zvalue value = FUN_CALL(testFunction);

    if (value == NULL) {
        die("Void result from `cases` call to `testFunction`.");
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
CMETH_IMPL_2_opt(If, is, testFunction, isFunction, notFunction) {
    if (FUN_CALL(testFunction) != NULL) {
        return FUN_CALL(isFunction);
    } else if (notFunction != NULL) {
        return FUN_CALL(notFunction);
    } else {
        return NULL;
    }
}

// Documented in spec.
CMETH_IMPL_1(If, loop, function) {
    for (;;) {
        zstackPointer save = datFrameStart();
        FUN_CALL(function);
        datFrameReturn(save, NULL);
    }
}

// Documented in spec.
CMETH_IMPL_1(If, loopUntil, function) {
    zvalue result = NULL;

    while (result == NULL) {
        zstackPointer save = datFrameStart();
        result = FUN_CALL(function);
        datFrameReturn(save, result);
    }

    return result;
}

// Documented in spec.
CMETH_IMPL_1(If, maybeValue, function) {
    zvalue value = FUN_CALL(function);
    return (value == NULL) ? EMPTY_LIST : listFromValue(value);
}

// Documented in spec.
CMETH_IMPL_2(If, not, testFunction, notFunction) {
    if (FUN_CALL(testFunction) == NULL) {
        return FUN_CALL(notFunction);
    } else {
        return NULL;
    }
}

// Documented in spec.
CMETH_IMPL_rest(If, or, functions) {
    for (zint i = 0; i < functions.size; i++) {
        zvalue result = FUN_CALL(functions.elems[i]);
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

// Documented in spec.
CMETH_IMPL_2_opt(If, value, testFunction, valueFunction, voidFunction) {
    zvalue result = FUN_CALL(testFunction);

    if (result != NULL) {
        return FUN_CALL(valueFunction, result);
    } else if (voidFunction != NULL) {
        return FUN_CALL(voidFunction);
    } else {
        return NULL;
    }
}

/** Initializes the module. */
MOD_INIT(If) {
    MOD_USE(Core);

    CLS_If = makeCoreClass(SYM(If), CLS_Core,
        METH_TABLE(
            CMETH_BIND(If, and),
            CMETH_BIND(If, andThenElse),
            CMETH_BIND(If, cases),
            CMETH_BIND(If, is),
            CMETH_BIND(If, loop),
            CMETH_BIND(If, loopUntil),
            CMETH_BIND(If, maybeValue),
            CMETH_BIND(If, not),
            CMETH_BIND(If, or),
            CMETH_BIND(If, value)),
        NULL);
}

// Documented in header.
zvalue CLS_If = NULL;
