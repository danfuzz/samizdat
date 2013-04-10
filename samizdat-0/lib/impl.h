/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Library implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lang.h"

/**
 * Binds all the primitive function definitions into the given
 * context.
 */
void bindPrimitives(zcontext ctx);

#endif
