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
 * Gets the payload data of the given arbitrary value. This handles both
 * core and derived values.
 */
zvalue constDataOf(zvalue value);

/**
 * Initializes the constants, if necessary.
 */
void constInit(void);

/**
 * Converts a C `zint` to an in-model int value.
 */
zvalue constIntFromZint(zint value);

/**
 * Gets the string representing the low-layer type of the given value.
 */
zvalue constLowTypeName(zvalue value);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue constStringFromZchar(zchar value);

/**
 * Returns true iff the type of the given arbitrary value is as given. This
 * handles both core and derived values.
 */
bool constTypeIs(zvalue value, zvalue type);

/**
 * Gets the type of the given arbitrary value. This handles both core and
 * derived values.
 */
zvalue constTypeOf(zvalue value);

/**
 * Produces an arbitrary value given a type and optional data payload. If
 * `type` corresponds to a core type, and `data` is a value of that type,
 * then this returns `data`. If not, then this produces a derived value
 * by calling `datDerivFrom` on the two arguments, or returns a cached
 * pre-existing value if a suitable one exists.
 */
zvalue constValueFrom(zvalue type, zvalue data);

#endif
