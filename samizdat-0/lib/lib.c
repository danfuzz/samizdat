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
 * Runs the core library code baked into this executable, returning
 * whatever value it yields, which *should* be the set of exported
 * core library bindings (including whatever primitives are passed
 * through).
 */
static zvalue runLibrary(zcontext ctx) {
    zvalue programText =
        datStringletFromUtf8String(library_sam0, library_sam0_len);
    zvalue program = langTextToProgramNode(programText);
    zvalue function = langFunctionFromNode(ctx, program);

    return langCall(function, 0, NULL);
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext libNewContext(void) {
    zcontext ctx = primitiveContext();
    zvalue libraryMaplet = runLibrary(ctx);
    zcontext result = langCtxNew();

    langCtxBindAll(result, libraryMaplet);
    return result;
}
