/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Prefab constants.
 */

#ifndef _CONSTS_H_
#define _CONSTS_H_

#include "dat.h"

/*
 * Declare globals for all of the constants.
 */

#define STR(name, str) extern zvalue STR_##name

#define TOK(name, str) \
    STR(name, str); \
    extern zvalue TOK_##name

#include "consts-def.h"

#undef STR
#undef TOK

/**
 * Initializes the string and token global constants, if necessary.
 */
void constsInit(void);

#endif
