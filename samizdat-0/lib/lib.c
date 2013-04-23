/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Creates a `zcontext`, and binds all the primitive definitions into it.
 */
static zcontext primitiveContext(void) {
    zcontext ctx = langCtxNew();

    // These both could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    langCtxBind(ctx, "false", langFalse());
    langCtxBind(ctx, "true", langTrue());

    // Bind all the primitive functions.
    #define PRIM_FUNC(name) langCtxBindFunction(ctx, #name, prim_##name, NULL)
    #include "prim-def.h"

    // Include a binding for a maplet of all the primitive bindings
    // (other than this one, since values can't self-reference).
    langCtxBind(ctx, "PRIMLIB", langMapletFromCtx(ctx));

    return ctx;
}

/**
 * Runs the core library code baked into this executable, returning
 * whatever value it yields, which *should* be the set of exported
 * core library bindings (including whatever primitives are passed
 * through).
 */
static zvalue runLibrary(zcontext ctx) {
    zvalue programText =
        datStringletFromUtf8String(library_sam0_len, library_sam0);
    zvalue program = langNodeFromProgramText(programText);
    zvalue function = langEvalExpressionNode(ctx, program);

    return langCall(function, 0, NULL);
}


/*
 * Module functions
 */

/** Documented in header. */
void requireExactly(zint argCount, zint required) {
    if (argCount != required) {
        die("Invalid argument count for primitive: %lld != %lld",
            argCount, required);
    }
}

/** Documented in header. */
void requireRange(zint argCount, zint min, zint max) {
    if (argCount < min) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, min);
    } else if (argCount > max) {
        die("Invalid argument count for primitive: %lld > %lld",
            argCount, max);
    }
}

/** Documented in header. */
void requireMinimum(zint argCount, zint minimum) {
    if (argCount < minimum) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, minimum);
    }
}

/** Documented in header. */
void requireEven(zint argCount) {
    if ((argCount & 0x01) != 0) {
        die("Invalid non-even argument count for primitive: %lld", argCount);
    }
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
