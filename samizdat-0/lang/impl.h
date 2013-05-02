/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Private structure implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lang.h"


/*
 * Execution Contexts
 */

/**
 * Allocates an execution context set up with the given parent bindings
 * and initial local variable bindings.
 */
zcontext ctxNewChild(zvalue parent, zvalue locals);


/*
 * Other
 */

/**
 * Tokenizes a stringlet using Samizdat Layer 0 token syntax. Returns
 * a listlet of tokens.
 */
zvalue tokenize(zvalue stringlet);


#endif
