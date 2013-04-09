/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "impl.h"
#include "util.h"

/* Documented in header. */
zvalue valueToken(zvalue token, zvalue value) {
    return datMapletPut(token, STR_VALUE, value);
}

/* Documented in header. */
zvalue hidType(zvalue value) {
    return datMapletGet(value, STR_TYPE);
}

/* Documented in header. */
zvalue hidValue(zvalue value) {
    return datMapletGet(value, STR_VALUE);
}

/* Documented in header. */
bool hidHasType(zvalue value, zvalue type) {
    return (datCompare(hidType(value), type) == 0);
}

/* Documented in header. */
void hidAssertType(zvalue value, zvalue type) {
    if (!hidHasType(value, type)) {
        die("Type mismatch.");
    }
}
