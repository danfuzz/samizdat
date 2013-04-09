/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "prim.h"
#include "util.h"


/*
 * Function implementations (exported via the context)
 */

static zvalue prim_if(zint argCount, zvalue *args) {
    die("TODO");
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext primNewContext(void) {
    zcontext ctx = langNewContext();

    // TODO: Stuff.

    return ctx;
}
