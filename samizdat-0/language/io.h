/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * I/O
 */

#ifndef IO_H
#define IO_H

#include "sam-data.h"

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a stringlet (listlet of Unicode-representing
 * intlets) of the contents.
 */
zvalue readFile(zvalue fileName);


#endif
