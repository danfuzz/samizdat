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
zvalue constDataOf(zvalue value) {
    if (datTypeIs(value, DAT_Deriv)) {
        return datDerivData(value);
    } else {
        return value;
    }
}

/* Documented in header. */
bool constTypeIs(zvalue value, zvalue type) {
    return datEq(datTypeOf(value), type);
}
