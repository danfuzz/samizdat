// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Commonly-used in-model constants.
//

#ifndef _LANGNODE_CONSTS_H_
#define _LANGNODE_CONSTS_H_

#include "type/declare.h"

//
// Declare globals for all of the constants.
//

#define DEF_LITERAL(name, value) \
    extern zvalue LIT_##name \
    // No semicolon here, so that use sites require it.

#define DEF_STRING(name, str) \
    extern zvalue STR_##name \
    // No semicolon here, so that use sites require it.

#define DEF_TOKEN(name) \
    SYM_DECL(name); \
    extern zvalue TOK_##name \
    // No semicolon here, so that use sites require it.

#define DEF_SYMBOL(name, str) \
    DEF_TOKEN(name)
    // No semicolon here, so that use sites require it.

#include "langnode/consts-def.h"

#undef DEF_LITERAL
#undef DEF_STRING
#undef DEF_TOKEN
#undef DEF_SYMBOL


#endif
