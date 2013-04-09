/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Tokenization.
 */

#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "dat.h"

/**
 * Tokenizes a stringlet using Samizdat Layer 0 token syntax. Returns
 * a listlet of tokens.
 */
zvalue tokenize(zvalue stringlet);

#endif
