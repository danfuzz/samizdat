// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/define.h"
#include "type/symbols.h"


//
// Define globals for all of the symbols.
//

#define DEF_SYMBOL(name) SYM_DEF(name)
#include "dat/symbols-def.h"
#undef DEF_SYMBOL

// Documented in header.
void initCoreSymbols(void) {
    #define DEF_SYMBOL(name) SYM_INIT(name)
    #include "dat/symbols-def.h"
    #undef DEF_SYMBOL
}
