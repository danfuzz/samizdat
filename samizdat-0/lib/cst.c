/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "lang.h"

#include <stddef.h>

zvalue CST_STR_INTLET = NULL;
zvalue CST_STR_STRINGLET = NULL;
zvalue CST_STR_LISTLET = NULL;
zvalue CST_STR_MAPLET = NULL;
zvalue CST_STR_UNIQLET = NULL;
zvalue CST_STR_HIGHLET = NULL;

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

    CST_STR_INTLET    = datStringletFromUtf8String(-1, "intlet");
    CST_STR_STRINGLET = datStringletFromUtf8String(-1, "stringlet");
    CST_STR_LISTLET   = datStringletFromUtf8String(-1, "listlet");
    CST_STR_MAPLET    = datStringletFromUtf8String(-1, "maplet");
    CST_STR_UNIQLET   = datStringletFromUtf8String(-1, "uniqlet");
    CST_STR_HIGHLET   = datStringletFromUtf8String(-1, "highlet");

    CST_FALSE = langFalse();
    CST_TRUE  = langTrue();
}
