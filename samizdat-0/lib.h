/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Samizdat Layer 0 core library
 */

#ifndef _LIB_H_
#define _LIB_H_

#include "lang.h"

/**
 * Constructs and returns a fresh execution context, containing
 * bindings for all the core library values and functions.
 */
zcontext libNewContext(void);

#endif
