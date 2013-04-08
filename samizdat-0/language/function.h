/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Function registry and lookup
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#include "data.h"

/**
 * Function registry. The contents of a registry are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct FunctionRegistry *zfunreg;

/**
 * Prototype of all bound functions.
 */
typedef zvalue (*zfunction)(void *state, zint argCount, const zvalue *args);

/**
 * Constructs and returns a new function registry.
 */
zfunreg funNew(void);

/**
 * Adds a function with associated (and arbitrary) closure
 * state. Returns the identifying uniqlet that binds to it.
 */
zvalue funAdd(zfunreg reg, zfunction function, void *state);

/**
 * Calls the function bound to the given uniqlet.
 */
zvalue funCall(zfunreg reg, zvalue id, zint argCount, const zvalue *args);

#endif
