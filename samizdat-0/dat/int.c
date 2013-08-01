/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Helper definitions
 */

enum {
    /** Ints are restricted to being in the range of `int32_t`. */
    MAX_BITS = 32,

    /** Max (exclusive) small int value. */
    DAT_SMALL_INT_MAX = DAT_SMALL_INT_MIN + DAT_SMALL_INT_COUNT
};

/** Array of small integer values. */
static zvalue SMALL_INTS[DAT_SMALL_INT_COUNT];

/**
 * Int structure.
 */
typedef struct {
    /** Int value. See `datIntFromZint()` about range restriction. */
    int32_t value;
} DatInt;

/**
 * Gets the bit size (highest-order significant bit number, plus one)
 * of the given `zint`.
 */
static zint bitSize(zint value) {
    if (value < 0) {
        value = ~value;
    }

    // "Binary-search" style implementation. Many compilers have a
    // built-in "count leading zeroes" function, but we're aiming
    // for portability here.

    zint result = 1; // +1 in that we want size, not zero-based bit number.
    uint64_t uv = (uint64_t) value; // Use `uint` to account for `-MAX_ZINT`.

    if (uv >= ((zint) 1 << 32)) { result += 32; uv >>= 32; }
    if (uv >= ((zint) 1 << 16)) { result += 16; uv >>= 16; }
    if (uv >= ((zint) 1 << 8))  { result +=  8; uv >>=  8; }
    if (uv >= ((zint) 1 << 4))  { result +=  4; uv >>=  4; }
    if (uv >= ((zint) 1 << 2))  { result +=  2; uv >>=  2; }
    if (uv >= ((zint) 1 << 1))  { result +=  1; uv >>=  1; }
    return result + uv;
}

/**
 * Gets the value of the given int as a `zint`. Doesn't do any
 * type checking.
 */
static zint zintValue(zvalue intval) {
    return ((DatInt *) datPayload(intval))->value;
}

/**
 * Constructs and returns an int.
 */
zvalue intFrom(zint value) {
    zint size = bitSize(value);
    zvalue result = datAllocValue(DAT_Int, sizeof(int32_t));

    if (size > MAX_BITS) {
        die("Value too large to fit into int: %lld", value);
    }

    ((DatInt *) datPayload(result))->value = (int32_t) value;
    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
zchar datZcharFromInt(zvalue intval) {
    zint value = datZintFromInt(intval);

    if ((value < 0) || (value >= 0x100000000)) {
        die("Invalid int value for character: %lld", value);
    }

    return (zchar) value;
}

/* Documented in header. */
zvalue datIntFromZint(zint value) {
    if ((value >= DAT_SMALL_INT_MIN) && (value < DAT_SMALL_INT_MAX)) {
        return SMALL_INTS[value - DAT_SMALL_INT_MIN];
    } else {
        return intFrom(value);
    }
}

/* Documented in header. */
bool datIntGetBit(zvalue intval, zint n) {
    datAssertInt(intval);
    return datZintGetBit(zintValue(intval), n);
}

/* Documented in header. */
zint datZintFromInt(zvalue intval) {
    datAssertInt(intval);
    return zintValue(intval);
}

/* Documented in header. */
bool datZintGetBit(zint value, zint n) {
    if (n < 0) {
        die("Attempt to access negative bit index: %lld", n);
    } else if (n >= MAX_BITS) {
        n = MAX_BITS - 1;
    }

    return (bool) ((value >> n) & 1);
}


/*
 * Type binding
 */

/* Documented in header. */
static void intGcMark(zvalue intval) {
    // Nothing to do here.
}

/* Documented in header. */
static bool intEq(zvalue v1, zvalue v2) {
    return zintValue(v1) == zintValue(v2);
}

/* Documented in header. */
static zorder intOrder(zvalue v1, zvalue v2) {
    zint int1 = zintValue(v1);
    zint int2 = zintValue(v2);

    if (int1 < int2) {
        return ZLESS;
    } else if (int1 > int2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}

/* Documented in header. */
static zvalue Int_sizeOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue intval = args[0];

    return datIntFromZint(bitSize(zintValue(intval)));
}

/* Documented in header. */
void datBindInt(void) {
    datGenBindCore(genSizeOf, DAT_Int, Int_sizeOf, NULL);

    for (zint i = 0; i < DAT_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = intFrom(i + DAT_SMALL_INT_MIN);
        datImmortalize(SMALL_INTS[i]);
    }
}

/* Documented in header. */
static DatType INFO_Int = {
    .name = "Int",
    .typeOf = NULL,
    .gcMark = intGcMark,
    .gcFree = NULL,
    .eq = intEq,
    .order = intOrder
};
ztype DAT_Int = &INFO_Int;
