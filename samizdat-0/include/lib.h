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

#include "pb.h"

/**
 * Constructs and returns a fresh execution context, containing bindings for
 * all the core library values and functions. `libraryDir` is a filesystem path
 * which is expected to point at a directory containing all the in-language
 * library implementation files.
 */
zvalue libNewContext(const char *libraryDir);

#endif
