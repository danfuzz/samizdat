// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lib.h"
#include "type/declare.h"


// Declarations for all the primitive functions.
#define PRIM_DEF(name, value) /*empty*/
#define PRIM_FUNC(name, minArgs, maxArgs) FUN_IMPL_DECL(name)
#include "prim-def.h"
#undef PRIM_DEF
#undef PRIM_FUNC

#endif
