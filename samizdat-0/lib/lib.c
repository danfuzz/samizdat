/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "lib.h"
#include "impl.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Adds the bindings supplied by the library code baked into this
 * executable.
 */
static void addLibrary(zcontext ctx) {
    zvalue programText =
        datStringletFromUtf8String(library_sam0, library_sam0_len);
    zvalue program = langCompile(programText);
    zcontext childCtx = langCtxNewChild(ctx);

    langExecute(childCtx, program);
    zvalue result = langCallMain(childCtx, 0, NULL);

    langCtxBindAll(ctx, result);
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext libNewContext(void) {
    cstInit();

    zcontext ctx = langCtxNew();

    // These all could have been defined in-language, but we already
    // had to have them defined and accessible to C code, so we just
    // go ahead and export them here.
    langBind(ctx, "false", CST_FALSE);
    langBind(ctx, "true", CST_TRUE);

    bindPrimitives(ctx);
    addLibrary(ctx);

    return ctx;
}
