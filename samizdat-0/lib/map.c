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
        result = mapAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapDel) {
    zvalue result = args[0];

    if (argCount == 0) {
        // Need the assert, since we won't end up making any other map calls.
        datAssertMap(result);
    } else {
        for (zint i = 1; i < argCount; i++) {
            result = mapDel(result, args[i]);
        }
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapGet) {
    return mapGet(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapKeys) {
    zvalue map = args[0];
    zint size = pbSize(map);
    zmapping mappings[size];
    zvalue arr[size];

    datArrayFromMap(mappings, map);

    for (zint i = 0; i < size; i++) {
        arr[i] = mappings[i].key;
    }

    return datListFromArray(size, arr);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapNth) {
    return doNthStrict(mapNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapPut) {
    return mapPut(args[0], args[1], args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mappingKey) {
    return mappingKey(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mappingValue) {
    return mappingValue(args[0]);
}
