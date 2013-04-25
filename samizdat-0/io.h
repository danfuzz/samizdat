/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * I/O
 */

#ifndef _IO_H_
#define _IO_H_

#include "dat.h"

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a stringlet (listlet of Unicode-representing
 * intlets) of the contents.
 */
zvalue ioReadFile(zvalue fileName);

/**
 * Writes the given stringlet to the file with the given name, encoding
 * it as UTF-8.
 */
void ioWriteFile(zvalue fileName, zvalue text);

#endif
