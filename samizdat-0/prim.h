/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Primitive function library
 */

#ifndef _PRIM_H_
#define _PRIM_H_

#include "lang.h"

/**
 * Constructs and returns a fresh execution context, containing
 * bindings for all the primitive values and functions.
 */
zcontext primNewContext(void);

#endif
