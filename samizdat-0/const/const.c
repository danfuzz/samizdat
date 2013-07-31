/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
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

/* Documented in header. */
zvalue EMPTY_LIST = NULL;

/* Documented in header. */
zvalue EMPTY_MAP = NULL;

/** Array of single-character strings, for character codes `0..127`. */
static zvalue SINGLE_CHAR_STRINGS[128];

/**
 * Array of type-only values whose types are all single-character strings,
 * for character codes `0..127`. Used as the token input to tokenizers,
 * hence the name.
 */
static zvalue SINGLE_CHAR_TOKENS[128];

/** Array of small integer values. */
static zvalue SMALL_INTS[CONST_SMALL_INT_COUNT];

enum {
    /** Max (exclusive) small int value. */
    CONST_SMALL_INT_MAX = CONST_SMALL_INT_MIN + CONST_SMALL_INT_COUNT
};

/*
 * Module functions
 */

/* Documented in header. */
void constInit(void) {
    if (EMPTY_LIST != NULL) {
        return;
    }

    datInit();

    zstackPointer save = datFrameStart();

    #define STR(name, str) \
        STR_##name = datStringFromUtf8(-1, str); \
        datImmortalize(STR_##name)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = datDerivFrom(STR_##name, NULL); \
        datImmortalize(TOK_##name)

    #include "const-def.h"

    for (zchar ch = 0; ch < 128; ch++) {
        SINGLE_CHAR_STRINGS[ch] = datStringFromZchars(1, &ch);
        SINGLE_CHAR_TOKENS[ch] = datDerivFrom(SINGLE_CHAR_STRINGS[ch], NULL);
        datImmortalize(SINGLE_CHAR_STRINGS[ch]);
        datImmortalize(SINGLE_CHAR_TOKENS[ch]);
    }

    for (zint i = 0; i < CONST_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = datIntFromZint(i + CONST_SMALL_INT_MIN);
        datImmortalize(SMALL_INTS[i]);
    }

    EMPTY_LIST = datListFromArray(0, NULL);
    EMPTY_MAP = datMapEmpty();

    datImmortalize(EMPTY_LIST);
    datImmortalize(EMPTY_MAP);

    datFrameReturn(save, NULL);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}

/* Documented in header. */
zvalue constStringFromZchar(zchar value) {
    if (value < 128) {
        return SINGLE_CHAR_STRINGS[value];
    } else {
        return datStringFromZchars(1, &value);
    }
}

/* Documented in header. */
zvalue constValueFrom(zvalue type, zvalue data) {
    if (data == NULL) {
        if (datTypeIs(type, DAT_String) && (datSize(type) == 1)) {
            // This is a type-only value with a one-character string for
            // the type. We have many of these cached.
            zchar typeCh = datStringNth(type, 0);
            if (typeCh < 128) {
                return SINGLE_CHAR_TOKENS[typeCh];
            }
        }
    } else if (!datTypeIs(data, DAT_Deriv)) {
        if (datEq(type, datTypeOf(data))) {
            // `data` is a core value, and its low-layer type matches the
            // given `type`. This means that we are in fact looking at a
            // "reconstructed" core value and should just return it directly.
            return data;
        }
    }

    return datDerivFrom(type, data);
}

/* Documented in header. */
zvalue constIntFromZint(zint value) {
    if ((value >= CONST_SMALL_INT_MIN) && (value < CONST_SMALL_INT_MAX)) {
        return SMALL_INTS[value - CONST_SMALL_INT_MIN];
    } else {
        return datIntFromZint(value);
    }
}
