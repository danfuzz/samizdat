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
    constInit();

    if (datOrder(value, CONST_FALSE) == 0) {
        return false;
    } else if (datOrder(value, CONST_TRUE) == 0) {
        return true;
    }

    die("Not a boolean value.");
}

/* Documented in header. */
zvalue constBooleanFromBool(bool value) {
    constInit();
    return value ? CONST_TRUE : CONST_FALSE;
}
