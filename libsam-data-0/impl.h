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
struct sam_value {
    /** Magic number. */
    uint32_t magic;

    /** Data type. */
    sam_type type;

    /** Size. Meaning varies depending on `type`. */
    sam_int size;
};

/**
 * Intlet structure.
 */
typedef struct {
    /** Value header. */
    sam_value header;

    // TODO: Stuff goes here.
} impl_intlet;

/**
 * Listlet structure.
 */
typedef struct {
    /** Value header. */
    sam_value header;

    // TODO: Stuff goes here.
} impl_listlet;

/**
 * Maplet structure.
 */
typedef struct {
    /** Value header. */
    sam_value header;

    // TODO: Stuff goes here.
} impl_maplet;

/**
 * Uniquelet structure.
 */
typedef struct {
    /** Value header. */
    sam_value header;

    // TODO: Stuff goes here.
} impl_uniquelet;

#endif
