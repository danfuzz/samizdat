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

#include "lib.h"

/** Embedded library source code, written in Samizdat Layer 0. */
extern char library_sam0[];

/** Number of bytes in `lib_library_sam0`. */
extern unsigned int library_sam0_len;


/**
 * Check the given argument count for an exact required amount,
 * complaining if it doesn't match.
 */
void requireExactly(zint argCount, zint required);

/**
 * Check the given argument count for a range of acceptable values,
 * complaining if it doesn't match.
 */
void requireRange(zint argCount, zint min, zint max);

/**
 * Check that the given argument count is even, complaining if not.
 */
void requireEven(zint argCount);

/**
 * Used at the top of primitive functions, to supply the standard
 * function prototype.
 */
#define PRIM_IMPL(name) \
    zvalue prim_##name(void *state, zint argCount, const zvalue *args)

/* Declarations for all the primitive functions */
#define PRIM_FUNC(name) zvalue prim_##name(void *, zint, const zvalue *)
#include "prim-def.h"
#undef PRIM_FUNC


#endif
