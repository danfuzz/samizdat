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
zvalue CONST_FALSE = NULL;

/* Documented in header. */
zvalue CONST_TRUE = NULL;

/* Documented in header. */
zvalue EMPTY_LIST = NULL;

/* Documented in header. */
zvalue EMPTY_MAP = NULL;

/** Array of single-character strings for character codes `0..127`. */
zvalue SINGLE_CHARS[128];

/** Array of small integer values. */
zvalue SMALL_INTS[CONST_SMALL_INT_COUNT];

enum {
    /** Max (exclusive) small int value. */
    CONST_SMALL_INT_MAX = CONST_SMALL_INT_MIN + CONST_SMALL_INT_COUNT
};

/*
 * Module functions
 */

/* Documented in header. */
void constInit(void) {
    if (CONST_FALSE != NULL) {
        return;
    }

    #define STR(name, str) \
        STR_##name = datStringFromUtf8(-1, str); \
        datImmortalize(STR_##name)

    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = datTokenFrom(STR_##name, NULL); \
        datImmortalize(TOK_##name)

    #include "const-def.h"

    for (zchar ch = 0; ch < 128; ch++) {
        SINGLE_CHARS[ch] = datStringFromZchars(1, &ch);
        datImmortalize(SINGLE_CHARS[ch]);
    }

    for (zint i = 0; i < CONST_SMALL_INT_COUNT; i++) {
        SMALL_INTS[i] = datIntFromZint(i + CONST_SMALL_INT_MIN);
        datImmortalize(SMALL_INTS[i]);
    }

    CONST_FALSE = datTokenFrom(STR_BOOLEAN, constIntFromZint(0));
    CONST_TRUE  = datTokenFrom(STR_BOOLEAN, constIntFromZint(1));
    EMPTY_LIST = datListFromArray(0, NULL);
    EMPTY_MAP = datMapEmpty();

    datImmortalize(CONST_FALSE);
    datImmortalize(CONST_TRUE);
    datImmortalize(EMPTY_LIST);
    datImmortalize(EMPTY_MAP);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();
}

/* Documented in header. */
zvalue constStringFromZchar(zchar value) {
    if (value < 128) {
        return SINGLE_CHARS[value];
    } else {
        return datStringFromZchars(1, &value);
    }
}

/* Documented in header. */
zvalue constIntFromZint(zint value) {
    if ((value >= CONST_SMALL_INT_MIN) && (value < CONST_SMALL_INT_MAX)) {
        return SMALL_INTS[value - CONST_SMALL_INT_MIN];
    } else {
        return datIntFromZint(value);
    }
}
