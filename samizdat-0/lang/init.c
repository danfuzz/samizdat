/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "const.h"
#include "impl.h"


/*
 * Module Definitions
 */

/* Documented in header. */
void langInit(void) {
    if (TYPE_NonlocalExit != NULL) {
        return;
    }

    constInit();
    langBindClosure();
    langBindNonlocalExit();
}
