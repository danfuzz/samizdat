/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/String.h"
#include "type/Value.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Define globals for all of the constants.
 */

#define STR(name, str) zvalue STR_##name = NULL

#define TOK(name, str) \
    STR(name, str); \
    zvalue TOK_##name = NULL

#include "const/const-def.h"

#undef STR
#undef TOK


/*
 * Module Definitions
 */

/** Initializes the module. */
MOD_INIT(const) {
    MOD_USE(dat);

    zstackPointer save = pbFrameStart();

    #define STR(name, str) \
        STR_##name = stringFromUtf8(-1, str); \
        pbImmortalize(STR_##name)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = makeTransValue(STR_##name, NULL); \
        pbImmortalize(TOK_##name)

    #include "const/const-def.h"

    pbFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    pbGc();
}
