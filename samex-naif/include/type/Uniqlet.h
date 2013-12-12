/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Uniqlet` data type
 */

#ifndef _TYPE_UNIQLET_H_
#define _TYPE_UNIQLET_H_

#include "pb.h"


/** Type value for in-model type `Uniqlet`. */
extern zvalue TYPE_Uniqlet;

/**
 * Makes and returns a new uniqlet. Each call to this function is guaranteed
 * to produce a value unequal to any other uniqlet (in any given process).
 */
zvalue makeUniqlet(void);

#endif
