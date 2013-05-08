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
PRIM_IMPL(mapAdd) {
    zvalue result = EMPTY_MAP;

    for (zint i = 0; i < argCount; i++) {
        result = datMapAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapDel) {
    requireExactly(argCount, 2);
    return datMapDel(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapGet) {
    requireRange(argCount, 2, 3);

    zvalue result = datMapGet(args[0], args[1]);

    if (result == NULL) {
        return (argCount == 3) ? args[2] : NULL;
    } else {
        return result;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapHasKey) {
    requireExactly(argCount, 2);
    return constBooleanFromBool(datMapGet(args[0], args[1]) != NULL);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapNth) {
    return doNth(datMapNth, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapNthKey) {
    return doNth(datMapNthKey, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapNthValue) {
    return doNth(datMapNthValue, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapPut) {
    requireExactly(argCount, 3);
    return datMapPut(args[0], args[1], args[2]);
}
