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


/**
 * Tokenizes a string using Samizdat Layer 0 token syntax. Returns
 * a list of tokens.
 */
zvalue tokenize(zvalue string);


#endif
