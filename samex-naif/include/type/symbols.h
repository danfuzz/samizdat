// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Symbols needed when using core classes.
//

#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "type/declare.h"

//
// Declare globals for all of the symbols.
//

#define DEF_SYMBOL(name) \
    SYM_DECL(name)
    // No semicolon here, so that use sites require it.

#include "dat/symbols-def.h"

#undef DEF_SYMBOL

#endif
