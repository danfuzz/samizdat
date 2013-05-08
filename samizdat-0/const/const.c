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

/* Documented in header. */
zvalue CONST_FALSE = NULL;

/* Documented in header. */
zvalue CONST_TRUE = NULL;

/* Documented in header. */
zvalue EMPTY_LISTLET = NULL;

/* Documented in header. */
zvalue EMPTY_MAPLET = NULL;


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
        TOK_##name = datHighletFrom(STR_##name, NULL); \
        datImmortalize(TOK_##name)

    #include "const-def.h"

    CONST_FALSE = datHighletFrom(STR_BOOLEAN, datIntegerFromInt(0));
    CONST_TRUE  = datHighletFrom(STR_BOOLEAN, datIntegerFromInt(1));
    EMPTY_LISTLET = datListFromArray(0, NULL);
    EMPTY_MAPLET = datMapletEmpty();

    datImmortalize(CONST_FALSE);
    datImmortalize(CONST_TRUE);
    datImmortalize(EMPTY_LISTLET);
    datImmortalize(EMPTY_MAPLET);
}
