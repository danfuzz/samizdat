// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stddef.h>

#include "type/DerivedData.h"
#include "type/Symbol.h"
#include "type/String.h"
#include "type/Class.h"
#include "zlimits.h"


//
// Define globals for all of the constants.
//

#define DEF_STRING(name, str) zvalue STR_##name = NULL

#define DEF_DATA(name, str) \
    DEF_STRING(name, str); \
    zvalue CLS_##name = NULL

#define DEF_TOKEN(name, str) \
    DEF_DATA(name, str); \
    zvalue TOK_##name = NULL

#include "const/const-def.h"

#undef DEF_STRING
#undef DEF_DATA
#undef DEF_TOKEN


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(const) {
    zstackPointer save = datFrameStart();

    MOD_USE(Value);

    #define DEF_STRING(name, str) \
        STR_##name = stringFromUtf8(-1, str); \
        datImmortalize(STR_##name)

    #define DEF_DATA(name, str) \
        DEF_STRING(name, str); \
        CLS_##name = makeDerivedDataClass(makeInternedSymbol(STR_##name)); \
        datImmortalize(CLS_##name)

    #define DEF_TOKEN(name, str) \
        DEF_DATA(name, str); \
        TOK_##name = makeData(CLS_##name, NULL); \
        datImmortalize(TOK_##name)

    #include "const/const-def.h"

    datFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}
