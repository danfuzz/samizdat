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

#include "lib.h"


/** Prototype for any of the dat module `nth` style functions. */
typedef zvalue (*znth)(zvalue value, zint n);

/**
 * Used at the top of primitive functions, to supply the standard
 * function prototype.
 */
#define PRIM_IMPL(name) \
    zvalue PRIM_##name(zint argCount, const zvalue *args)

/* Declarations for all the primitive functions */
#define PRIM_DEF(name, value) /*empty*/
#define PRIM_FUNC(name, minArgs, maxArgs) \
    zvalue PRIM_##name(zint, const zvalue *)
#include "prim-def.h"
#undef PRIM_DEF
#undef PRIM_FUNC


#endif
