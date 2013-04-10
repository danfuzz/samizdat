/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "lib.h"
#include "impl.h"


/*
 * Exported functions
 */

/* Documented in header. */
zcontext libNewContext(void) {
    zcontext ctx = langNewContext();

    // These all could have been defined in-language, but we already
    // had to have them defined and accessible to C code, so we just
    // go ahead and export them here.
    langBind(ctx, "null", langNull());
    langBind(ctx, "false", langFalse());
    langBind(ctx, "true", langTrue());

    bindPrimitives(ctx);

    // TODO: Execute Samizdat library code.

    return ctx;
}
