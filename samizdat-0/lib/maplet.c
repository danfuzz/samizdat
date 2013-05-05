/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Primitive implementations (exported via the context)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletAdd) {
    zvalue result = EMPTY_MAPLET;

    for (zint i = 0; i < argCount; i++) {
        result = datMapletAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletDel) {
    requireExactly(argCount, 2);
    return datMapletDel(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletGet) {
    requireRange(argCount, 2, 3);

    zvalue result = datMapletGet(args[0], args[1]);

    if (result == NULL) {
        return (argCount == 3) ? args[2] : NULL;
    } else {
        return result;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletHasKey) {
    requireExactly(argCount, 2);
    return constBooleanFromBool(datMapletGet(args[0], args[1]) != NULL);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletNth) {
    return doNth(datMapletNth, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletNthKey) {
    return doNth(datMapletNthKey, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletNthValue) {
    return doNth(datMapletNthValue, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletPut) {
    requireExactly(argCount, 3);
    return datMapletPut(args[0], args[1], args[2]);
}
