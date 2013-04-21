/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"

#include <stddef.h>


/*
 * Define globals for all of the constants.
 */

#define STR(name, str) zvalue STR_##name = NULL

#define TOK(name, str) \
    STR(name, str); \
    zvalue TOK_##name = NULL

#include "consts-def.h"

#undef STR
#undef TOK


/*
 * Module functions
 */

/* Documented in header. */
void constsInit(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    #define STR(name, str) STR_##name = datStringletFromUtf8String(str, -1)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = datHighletFrom(STR_##name, NULL);

    #include "consts-def.h"
}
