/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "prim.h"
#include "util.h"

#include <stddef.h>


/*
 * Primitive implementations (exported via the context)
 */

static zvalue prim_if(void *state, zint argCount, const zvalue *args) {
    if ((argCount % 2) != 0) {
        die("Invalid argument count to if: %lld", argCount);
    }

    for (zint i = 0; i < argCount; i += 2) {
        zvalue test = langCall(args[i], 0, NULL);
        if (datCompare(test, langFalse()) != 0) {
            return langCall(args[i + 1], 0, NULL);
        }
    }

    return langNull();
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext primNewContext(void) {
    zcontext ctx = langNewContext();

    // These all could have been defined in-language, but we already
    // had to have them defined and accessible to C code, so we just
    // go ahead and export them here.
    langBind(ctx, "null", langNull());
    langBind(ctx, "false", langFalse());
    langBind(ctx, "true", langTrue());

    langBindFunction(ctx, "if", prim_if, NULL);
    // TODO: More.

    return ctx;
}
