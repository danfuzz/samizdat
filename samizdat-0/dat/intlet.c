/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Helper functions
 */

enum {
    /** Intlets are restricted to being in the range of `int32_t`. */
    MAX_BITS = 32
};

/**
 * Gets the value of the given intlet as a `zint`. Doesn't do any
 * type checking.
 */
static zint intletValue(zvalue intlet) {
    return ((DatIntlet *) intlet)->value;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datIntletOrder(zvalue v1, zvalue v2) {
    zint int1 = intletValue(v1);
    zint int2 = intletValue(v2);

    if (int1 < int2) {
        return ZLESS;
    } else if (int1 > int2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
bool datIntletGetBit(zvalue intlet, zint n) {
    datAssertIntlet(intlet);

    if (n >= MAX_BITS) {
        n = MAX_BITS - 1;
    }

    return (bool) ((intletValue(intlet) >> n) & 1);
}

/* Documented in header. */
bool datIntletSign(zvalue intlet) {
    return datIntletGetBit(intlet, MAX_BITS - 1);
}

/* Documented in header. */
zvalue datIntletFromInt(zint value) {
    zint bitSize = 32; // FIXME!
    zvalue result = datAllocValue(DAT_INTLET, bitSize, sizeof(int32_t));

    ((DatIntlet *) result)->value = (int32_t) value;
    return result;
}

/* Documented in header. */
zint datIntletToInt(zvalue intlet) {
    datAssertIntlet(intlet);
    return intletValue(intlet);
}
