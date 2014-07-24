// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stddef.h>

#include "type/DerivedData.h"
#include "type/String.h"
#include "type/Class.h"
#include "zlimits.h"


//
// Define globals for all of the constants.
//

#define STR(name, str) zvalue STR_##name = NULL

#define TYP(name, str) \
    STR(name, str); \
    zvalue TYPE_##name = NULL

#define TOK(name, str) \
    TYP(name, str); \
    zvalue TOK_##name = NULL

#include "const/const-def.h"

#undef STR
#undef TOK
#undef TYP


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(const) {
    zstackPointer save = datFrameStart();

    MOD_USE(Value);

    #define STR(name, str) \
        STR_##name = stringFromUtf8(-1, str); \
        datImmortalize(STR_##name)

    #define TYP(name, str) \
        STR(name, str); \
        TYPE_##name = makeDerivedDataType(STR_##name); \
        datImmortalize(TYPE_##name)

    #define TOK(name, str) \
        TYP(name, str); \
        TOK_##name = makeData(TYPE_##name, NULL); \
        datImmortalize(TOK_##name)

    #include "const/const-def.h"

    datFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}
