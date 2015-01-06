// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Lowest level type definitions
//

#ifndef _ZTYPE_H_
#define _ZTYPE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * 64-bit integer. This is the type used for all lowest-level integer
 * values.
 */
typedef int64_t zint;

/**
 * 32-bit unsigned integer. This is the type used to represent
 * characters individually (as UTF-32 values / naked Unicode code
 * points).
 */
typedef uint32_t zchar;

/**
 * 1-bit boolean. This is the type used as the underlying representation
 * of in-model boolean values.
 */
typedef bool zbool;

enum {
    /** Number of bits in type `zchar`. */
    ZCHAR_BITS = sizeof(zchar) * 8,

    /** Maximum value of type `zchar`. */
    ZCHAR_MAX = UINT32_MAX,

    /** Number of bits in type `zint`. */
    ZINT_BITS = sizeof(zint) * 8,

    /** Maximum value of type `zint`. */
    ZINT_MAX = INT64_MAX,

    /** Minimum value of type `zint`. */
    ZINT_MIN = INT64_MIN
};

/**
 * The result of a comparison, which is to say, an order.
 */
typedef enum {
    ZLESS = -1,
    ZSAME = 0,
    ZMORE = 1
} zorder;

#endif
