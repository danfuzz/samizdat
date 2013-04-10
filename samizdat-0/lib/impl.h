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

/** Embedded library source code, written in Samizdat Layer 0. */
extern char lib_library_sam0[];

/** Number of bytes in `lib_library_sam0`. */
extern unsigned int lib_library_sam0_len;

/**
 * Binds all the primitive function definitions into the given
 * context.
 */
void bindPrimitives(zcontext ctx);

#endif
