/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

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

/**
 * Array of type-only values whose types are all single-character strings,
 * for character codes `0..127`. Used as the token input to tokenizers,
 * hence the name.
 */
static zvalue SINGLE_CHAR_TOKENS[128];


/*
 * Module functions
 */

/* Documented in header. */
void constInit(void) {
    if (STR_DEF != NULL) {
        return;
    }

    datInit();

    zstackPointer save = pbFrameStart();

    #define STR(name, str) \
        STR_##name = stringFromUtf8(-1, str); \
        pbImmortalize(STR_##name)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = valueFrom(STR_##name, NULL); \
        pbImmortalize(TOK_##name)

    #include "const-def.h"

    for (zchar ch = 0; ch < 128; ch++) {
        SINGLE_CHAR_TOKENS[ch] = valueFrom(stringFromZchar(ch), NULL);
        pbImmortalize(SINGLE_CHAR_TOKENS[ch]);
    }

    generatorInit();

    pbFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    pbGc();
}

/* Documented in header. */
zvalue constValueFrom(zvalue type, zvalue data) {
    if (data == NULL) {
        if (hasType(type, TYPE_String) && (pbSize(type) == 1)) {
            // This is a type-only value with a one-character string for
            // the type. We have many of these cached.
            zchar typeCh = stringNth(type, 0);
            if (typeCh < 128) {
                return SINGLE_CHAR_TOKENS[typeCh];
            }
        }
    }

    return valueFrom(type, data);
}
