/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "lib.h"
#include "impl.h"


/*
 * Helper functions
 */

/**
 * Runs the library code baked into this executable.
 */
static void runLibrary(zcontext ctx) {
    zvalue programText =
        datStringletFromUtf8String(lib_library_sam0, lib_library_sam0_len);
    zvalue program = langCompile(programText);

    langExecute(ctx, program);
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext libNewContext(void) {
    cstInit();

    zcontext ctx = langNewContext();

    // These all could have been defined in-language, but we already
    // had to have them defined and accessible to C code, so we just
    // go ahead and export them here.
    langBind(ctx, "null", CST_NULL);
    langBind(ctx, "false", CST_FALSE);
    langBind(ctx, "true", CST_TRUE);

    bindPrimitives(ctx);
    runLibrary(ctx);

    return ctx;
}
