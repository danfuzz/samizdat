/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Private structure implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "sam-data.h"

enum {
    /** "Magic number" for value validation. */
    SAM_VALUE_MAGIC = 0x600f1e57;
};

/**
 * Common fields across all values. Used as a header for other types.
 */
typedef struct SamValue {
    /** Magic number. */
    uint32_t magic;

    /** Data type. */
    ztype type;

    /** Size. Meaning varies depending on `type`. */
    zint size;
} SamValue;

/**
 * Intlet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    // TODO: Stuff goes here.
} SamIntlet;

/**
 * Listlet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    // TODO: Stuff goes here.
} SamListlet;

/**
 * Maplet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    // TODO: Stuff goes here.
} SamMaplet;

/**
 * Uniquelet structure.
 */
typedef struct {
    /** Value header. */
    SamValue header;

    // TODO: Stuff goes here.
} SamUniquelet;

#endif
