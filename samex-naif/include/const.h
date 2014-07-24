// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Commonly-used in-model constants.
//

#ifndef _CONST_H_
#define _CONST_H_

#include "dat.h"

//
// Declare globals for all of the constants.
//

#define STR(name, str) extern zvalue STR_##name

#define TYP(name, str) \
    STR(name, str); \
    extern zvalue CLS_##name

#define TOK(name, str) \
    TYP(name, str); \
    extern zvalue TOK_##name

#include "const/const-def.h"

#undef STR
#undef TYP
#undef TOK


#endif
