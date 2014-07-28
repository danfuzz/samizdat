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

#define DEF_STRING(name, str) extern zvalue STR_##name

#define DEF_DATA(name, str) \
    DEF_STRING(name, str); \
    extern zvalue CLS_##name

#define DEF_TOKEN(name, str) \
    DEF_DATA(name, str); \
    extern zvalue TOK_##name

#include "const/const-def.h"

#undef DEF_STRING
#undef DEF_DATA
#undef DEF_TOKEN


#endif
