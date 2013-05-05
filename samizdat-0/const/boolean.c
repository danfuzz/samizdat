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
bool constBoolFromBoolean(zvalue value) {
    if (datEq(value, CONST_FALSE)) {
        return false;
    } else if (datEq(value, CONST_TRUE)) {
        return true;
    }

    die("Not a boolean value.");
}

/* Documented in header. */
zvalue constBooleanFromBool(bool value) {
    return value ? CONST_TRUE : CONST_FALSE;
}
