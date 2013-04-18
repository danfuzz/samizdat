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

/* Prefab stringlets. */
extern zvalue CST_STR_INTLET;
extern zvalue CST_STR_STRINGLET;
extern zvalue CST_STR_LISTLET;
extern zvalue CST_STR_MAPLET;
extern zvalue CST_STR_UNIQLET;
extern zvalue CST_STR_HIGHLET;

/* Other constants. */
extern zvalue CST_TRUE;
extern zvalue CST_FALSE;

/**
 * Initializes the string and token variables, if necessary.
 */
void cstInit(void);

#endif
