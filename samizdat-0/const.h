/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Commonly-used in-model constants.
 */

#ifndef _CONST_H_
#define _CONST_H_

#include "dat.h"

/*
 * Declare globals for all of the constants.
 */

#define STR(name, str) extern zvalue STR_##name

#define TOK(name, str) \
    STR(name, str); \
    extern zvalue TOK_##name

#include "const/const-def.h"

#undef STR
#undef TOK

/** The canonical empty string value (`""`). */
extern zvalue STR_EMPTY;

/** The canonical empty list value (`[]`). */
extern zvalue EMPTY_LIST;

/** The canonical empty map value (`[:]`). */
extern zvalue EMPTY_MAP;

/**
 * Initializes the constants, if necessary.
 */
void constInit(void);

/**
 * Converts a C `zint` to an in-model int value.
 */
zvalue constIntFromZint(zint value);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue constStringFromZchar(zchar value);

/**
 * This is the same as `datTokenFrom`, except that in some cases, this
 * will reuse a pre-existing value.
 */
zvalue constTokenFrom(zvalue type, zvalue value);

/**
 * Gets the string representing the low-layer type of the given value.
 */
zvalue constLowTypeName(zvalue value);

#endif
