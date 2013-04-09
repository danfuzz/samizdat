/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Parsing (into trees)
 */

#ifndef _PARSE_H_
#define _PARSE_H_

#include "dat.h"

/**
 * Parses a listlet of tokens into Samizdat Layer 0 parse trees. Returns
 * a node representing the parsed program.
 */
zvalue parse(zvalue tokens);

#endif
