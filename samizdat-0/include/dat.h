/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Mid-layer data types
 */

#ifndef _DAT_H_
#define _DAT_H_

#include "pb.h"


/**
 * Initializes the `dat` module.
 */
void datInit(void);

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue funApply(zvalue function, zvalue args);

#endif
