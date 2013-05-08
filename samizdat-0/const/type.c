/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "util.h"


/*
 * Exported functions
 */

/* Documented in header. */
zvalue constLowTypeName(zvalue value) {
    switch (datType(value)) {
        case DAT_INTEGER:    return STR_INTEGER;
        case DAT_STRING: return STR_STRING;
        case DAT_LISTLET:   return STR_LISTLET;
        case DAT_MAPLET:    return STR_MAPLET;
        case DAT_UNIQLET:   return STR_UNIQLET;
        case DAT_HIGHLET:   return STR_HIGHLET;
        default: {
            die("Invalid value type (shouldn't happen): %d", datType(value));
        }
    }
}
