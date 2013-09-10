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

LIB_FILE(Bitwise,             sam0);
LIB_FILE(Boolean,             sam0);
LIB_FILE(Collection,          sam0);
LIB_FILE(CollectionGenerator, sam0);
LIB_FILE(FilterGenerator,     sam0);
LIB_FILE(FunctionForwarder,   sam0);
LIB_FILE(Generator,           sam0);
LIB_FILE(ListWrapGenerator,   sam0);
LIB_FILE(Module,              sam0);
LIB_FILE(NullBox,             sam0);
LIB_FILE(NullGenerator,       sam0);
LIB_FILE(OptGenerator,        sam0);
LIB_FILE(ParaGenerator,       sam0);
LIB_FILE(ParseForwarder,      sam0);
LIB_FILE(SerialGenerator,     sam0);
LIB_FILE(String,              sam0);
LIB_FILE(Value,               sam0);
LIB_FILE(ValueGenerator,      sam0);

LIB_FILE(constants,           sam0);
LIB_FILE(entityMap,           sam0);
LIB_FILE(eval1,               sam0);
LIB_FILE(format,              sam0);
LIB_FILE(main,                sam0);
LIB_FILE(manifest,            sam0);
LIB_FILE(meta,                sam0);
LIB_FILE(peg,                 sam0);
LIB_FILE(range,               sam0);
LIB_FILE(tokenize1,           sam0);
LIB_FILE(tree1,               sam0);

LIB_FILE(commandLine,         sam1);
LIB_FILE(eval2,               sam1);
LIB_FILE(io0,                 sam1);
LIB_FILE(tokenize2,           sam1);
LIB_FILE(tree2,               sam1);
