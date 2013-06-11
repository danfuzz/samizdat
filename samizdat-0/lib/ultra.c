/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeToken) {
    requireRange(argCount, 1, 2);

    zvalue value = (argCount == 2) ? args[1] : NULL;
    return datTokenFrom(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeList) {
    return datListFromArray(argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMap) {
    requireEven(argCount);

    zint size = argCount / 2;
    zmapping mappings[size];

    for (zint i = 0; i < size; i++) {
        mappings[i].key = args[i * 2];
        mappings[i].value = args[i * 2 + 1];
    }

    return datMapAddArray(EMPTY_MAP, size, mappings);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeRange) {
    requireExactly(argCount, 2);

    zvalue start = args[0];
    zvalue end = args[1];
    ztype type = datType(start);

    if (type != datType(end)) {
        die("Type mismatch on range.");
    }

    if (datOrder(start, end) > 0) {
        die("Bad order for range.");
    }

    // TODO: Handle more than just int ranges.
    if (type != DAT_INT) {
        die("Bad type for range.");
    }

    zint startInt = datZintFromInt(start);
    zint endInt = datZintFromInt(end);
    zint size = endInt - startInt + 1;
    zvalue values[size];

    for (zint i = 0; i < size; i++) {
        values[i] = constIntFromZint(startInt + i);
    }

    return datListFromArray(size, values);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeUniqlet) {
    requireExactly(argCount, 0);
    return datUniqlet();
}
