/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

/*
 * Module functions
 */

/* Documented in header. */
bool datHasNth(zvalue value, zint n) {
    return (n >= 0) && (n < datSize(value));
}


/*
 * Exported functions
 */

/* Documented in header. */
zint datSize(zvalue value) {
    datAssertValid(value);
    return value->size;
}

/* Documented in header. */
ztype datType(zvalue value) {
    datAssertValid(value);
    return value->type;
}

/* Documented in header. */
bool datTypeIs(zvalue value, ztype type) {
    datAssertValid(value);
    return value->type == type;
}
