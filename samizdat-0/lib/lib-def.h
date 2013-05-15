/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `LIB_FILE(name)` defines an in-language core library file with the given
 * name. These are all `.sam0` source files that get embedded into the
 * `samizdat-0` executable.
 *
 * **Note:** This file gets `#include`d as source, and so does not have the
 * usual guard macros.
 */

LIB_FILE(boolean);
LIB_FILE(combinator);
LIB_FILE(conditional);
LIB_FILE(collections);
LIB_FILE(const);
LIB_FILE(format);
LIB_FILE(int);
LIB_FILE(io0);
LIB_FILE(main);
LIB_FILE(meta);
LIB_FILE(type);
