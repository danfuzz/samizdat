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
extern char library_sam0[];

/** Number of bytes in `lib_library_sam0`. */
extern unsigned int library_sam0_len;

/**
 * Creates a context, and binds all the primitive definitions into it.
 */
zcontext primitiveContext(void);

#endif
