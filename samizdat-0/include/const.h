/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Commonly-used in-model constants.
 */

#ifndef _CONST_H_
#define _CONST_H_

#include "pb.h"

/*
 * Declare globals for all of the constants.
 */

#define STR(name, str) extern zvalue STR_##name

#define TOK(name, str) \
    STR(name, str); \
    extern zvalue TOK_##name

#include "const/const-def.h"

#undef STR
#undef TOK

/**
 * Initializes the constants, if necessary.
 */
void constInit(void);


#endif
