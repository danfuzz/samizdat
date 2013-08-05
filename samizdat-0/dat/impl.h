/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Private implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "dat.h"
#include "util.h"


/**
 * Clears the contents of the map lookup cache.
 */
void datMapClearCache(void);


/*
 * Initialization functions
 */

// Per-type generic binding.
void datBindBox(void);
void datBindDeriv(void);
void datBindList(void);
void datBindMap(void);
void datBindUniqlet(void);

#endif
