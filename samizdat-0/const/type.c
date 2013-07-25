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
        case DAT_DERIV:   return STR_DERIV;
        case DAT_INT:     return STR_INT;
        case DAT_STRING:  return STR_STRING;
        case DAT_LIST:    return STR_LIST;
        case DAT_MAP:     return STR_MAP;
        case DAT_UNIQLET: return STR_UNIQLET;
        default: {
            die("Invalid core type (shouldn't happen): %d", datType(value));
        }
    }
}
