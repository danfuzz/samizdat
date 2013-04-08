/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Private allocation-related functions
 */

#ifndef _ALLOC_H_
#define _ALLOC_H_

#include "data.h"

/**
 * Allocates zeroed-out memory of the indicated size (in bytes).
 */
void *samAlloc(zint size);

/**
 * Allocates memory, sized to include a `SamValue` header plus the
 * indicated number of extra bytes. The `SamValue` header is
 * initialized with the indicated type and size.
 */
zvalue samAllocValue(ztype type, zint size, zint extraBytes);

#endif
