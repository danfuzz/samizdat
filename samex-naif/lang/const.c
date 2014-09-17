// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stddef.h>

#include "type/Map.h"
#include "type/define.h"


//
// Define globals for all of the constants.
//

#define DEF_STRING(name, str) \
    zvalue STR_##name = NULL

#define DEF_SYMBOL(name, str) \
    SYM_DEF(name)

#define DEF_RECORD(name, str) \
    zvalue CLS_##name = NULL

#define DEF_TOKEN(name, str) \
    DEF_RECORD(name, str); \
    zvalue TOK_##name = NULL

#include "const-def.h"

#undef DEF_STRING
#undef DEF_SYMBOL
#undef DEF_RECORD
#undef DEF_TOKEN


//
// Module Definitions
//

/** Initializes the module. */
MOD_INIT(lang_const) {
    zstackPointer save = datFrameStart();

    MOD_USE(Value);

    #define DEF_STRING(name, str) \
        STR_##name = datImmortalize(stringFromUtf8(-1, str))

    #define DEF_SYMBOL(name, str) \
        SYM_INIT_WITH(name, str)

    #define DEF_RECORD(name, str) \
        CLS_##name = datImmortalize( \
            makeRecordClass(symbolFromUtf8(-1, str)));

    #define DEF_TOKEN(name, str) \
        DEF_RECORD(name, str); \
        TOK_##name = datImmortalize(makeRecord(CLS_##name, EMPTY_SYMBOL_TABLE));

    #include "const-def.h"

    datFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}
