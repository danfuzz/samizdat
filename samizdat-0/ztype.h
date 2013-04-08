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
    SAM_IS_LESS = -1,
    SAM_IS_EQUAL = 0,
    SAM_IS_MORE = 1
} zcomparison;

#endif
