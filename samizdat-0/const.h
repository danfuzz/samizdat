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

/** The canonical `false` value. */
extern zvalue CONST_FALSE;

/** The canonical `true` value. */
extern zvalue CONST_TRUE;

/** The canonical empty string value (`@""`). */
extern zvalue STR_EMPTY;

/** The canonical empty list value (`@[]`). */
extern zvalue EMPTY_LIST;

/** The canonical empty map value (`@[=]`). */
extern zvalue EMPTY_MAP;

/**
 * Initializes the constants, if necessary.
 */
void constInit(void);

/**
 * Converts an in-model boolean value to a C `bool`.
 */
bool constBoolFromBoolean(zvalue value);

/**
 * Converts a C `bool` to an in-model boolean value.
 */
zvalue constBooleanFromBool(bool value);

/**
 * Gets the string representing the low-layer type of the given value.
 */
zvalue constLowTypeName(zvalue value);

#endif
