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
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeList) {
    if (argCount == 0) {
        return EMPTY_LIST;
    }

    return datListFromArray(argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMapping) {
    requireAtLeast(argCount, 1);

    zint size = argCount - 1;
    zvalue keys = args[0];
    zvalue value = args[size];

    if (size == 0) {
        return EMPTY_MAP;
    }

    zmapping mappings[size];

    for (zint i = 0; i < size; i++) {
        mappings[i].key = args[i];
        mappings[i].value = value;
    }

    return datMapAddArray(EMPTY_MAP, size, mappings);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeValue) {
    requireRange(argCount, 1, 2);

    zvalue value = (argCount == 2) ? args[1] : NULL;
    return constValueFrom(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeUniqlet) {
    requireExactly(argCount, 0);
    return datUniqlet();
}
