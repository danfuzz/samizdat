/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Lowest level type definitions
 */

#ifndef ZTYPE_H
#define ZTYPE_H

#include <stdint.h>

/**
 * 64-bit integer. This is the type used for all lowest-level integer
 * values.
 */
typedef int64_t zint;

/**
 * The result of a comparison.
 */
typedef enum {
    ZLESS = -1,
    ZEQUAL = 0,
    ZMORE = 1
} zcomparison;

#endif
