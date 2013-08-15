/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `LIB_FILE(name, ext)` defines an in-language core library file with the
 * given name and file extension. These are all Samizdat source files that
 * get embedded into the `samizdat-0` executable.
 *
 * **Note:** This file gets `#include`d as source, and so does not have the
 * usual guard macros.
 */

LIB_FILE(Boolean,      sam0);
LIB_FILE(List,         sam0);
LIB_FILE(String,       sam0);
LIB_FILE(Type,         sam0);

LIB_FILE(callable,     sam0);
LIB_FILE(const,        sam0);
LIB_FILE(coreDispatch, sam0);
LIB_FILE(entityMap,    sam0);
LIB_FILE(eval1,        sam0);
LIB_FILE(format,       sam0);
LIB_FILE(generator,    sam0);
LIB_FILE(main,         sam0);
LIB_FILE(manifest,     sam0);
LIB_FILE(meta,         sam0);
LIB_FILE(module,       sam0);
LIB_FILE(order,        sam0);
LIB_FILE(peg,          sam0);
LIB_FILE(tokenize1,    sam0);
LIB_FILE(tree1,        sam0);

LIB_FILE(commandLine,  sam1);
LIB_FILE(eval2,        sam1);
LIB_FILE(io0,          sam1);
LIB_FILE(tokenize2,    sam1);
LIB_FILE(tree2,        sam1);

LIB_FILE(operators,    sam2);
