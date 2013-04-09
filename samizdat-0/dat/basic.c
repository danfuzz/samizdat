/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

/* Documented in header. */
ztype datType(zvalue value) {
    datAssertValid(value);
    return value->type;
}

/* Documented in header. */
zint datSize(zvalue value) {
    datAssertValid(value);
    return value->size;
}
