/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

/** Documented in API header. */
ztype samType(zvalue value) {
    samAssertValid(value);
    return value->type;
}

/** Documented in API header. */
zint samSize(zvalue value) {
    samAssertValid(value);
    return value->size;
}
