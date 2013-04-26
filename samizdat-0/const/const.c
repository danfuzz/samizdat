/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"

#include <stddef.h>


/*
 * Define globals for all of the constants.
 */

#define STR(name, str) zvalue STR_##name = NULL

#define TOK(name, str) \
    STR(name, str); \
    zvalue TOK_##name = NULL

#include "const-def.h"

#undef STR
#undef TOK


/*
 * Module functions
 */

/* Documented in header. */
void constInit(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    #define STR(name, str) STR_##name = datStringletFromUtf8String(-1, str)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = datHighletFrom(STR_##name, NULL);

    #include "const-def.h"
}
