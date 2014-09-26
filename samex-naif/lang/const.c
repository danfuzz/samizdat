// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stddef.h>

#include "type/Map.h"
#include "type/define.h"

#include "const.h"


//
// Define globals for all of the constants.
//

#define DEF_STRING(name, str) \
    zvalue STR_##name = NULL

#define DEF_RECORD(name, str) \
    zvalue RECNAME_##name = NULL; \
    zvalue TOK_##name = NULL

#define DEF_SYMBOL(name, str) \
    SYM_DEF(name); \
    DEF_RECORD(name, str)

#include "const-def.h"

#undef DEF_STRING
#undef DEF_RECORD
#undef DEF_SYMBOL


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(lang_const) {
    zstackPointer save = datFrameStart();

    MOD_USE(Value);

    #define DEF_STRING(name, str) \
        STR_##name = datImmortalize(stringFromUtf8(-1, str))

    #define DEF_RECORD(name, str) \
        RECNAME_##name = SYM(name); \
        TOK_##name = datImmortalize( \
            makeRecord(RECNAME_##name, EMPTY_SYMBOL_TABLE))

    #define DEF_SYMBOL(name, str) \
        SYM_INIT_WITH(name, str); \
        DEF_RECORD(name, str)

    #include "const-def.h"

    datFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}
