/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "lang.h"

#include <stddef.h>

zvalue CST_FALSE = NULL;
zvalue CST_TRUE = NULL;


/*
 * Module functions
 */

/* Documented in header. */
void cstInit(void) {
    if (CST_TRUE != NULL) {
        return;
    }

    CST_FALSE = langFalse();
    CST_TRUE  = langTrue();
}
