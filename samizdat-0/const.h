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

/**
 * Returns a collected list of items from the given value, which must either
 * be a generator or a collection value. This is a C equivalent to calling
 * `collectGenerator(value)`, hence the name.
 */
zvalue constCollectGenerator(zvalue value);

/**
 * Initializes the constants, if necessary.
 */
void constInit(void);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue constStringFromZchar(zchar value);

/**
 * Produces an arbitrary value given a type and optional data payload. If
 * `type` corresponds to a core type, and `data` is a value of that type,
 * then this returns `data`. If not, then this produces a derived value
 * by calling `derivFrom` on the two arguments, or returns a cached
 * pre-existing value if a suitable one exists.
 */
zvalue constValueFrom(zvalue type, zvalue data);


#endif
