/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Exported functions
 */

/* Documented in header. */
bool langBoolFromBoolean(zvalue value) {
    constInit();

    if (datOrder(value, CONST_FALSE) == 0) {
        return false;
    } else if (datOrder(value, CONST_TRUE) == 0) {
        return true;
    }

    die("Not a boolean value.");
}

/* Documented in header. */
zvalue langBooleanFromBool(bool value) {
    constInit();
    return value ? CONST_TRUE : CONST_FALSE;
}
