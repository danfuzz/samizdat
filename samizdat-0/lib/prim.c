/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "io.h"
#include "impl.h"
#include "lib.h"
#include "util.h"

#include <stddef.h>


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
void requireEven(zint argCount) {
    if ((argCount & 0x01) != 0) {
        die("Invalid non-even argument count for primitive: %lld", argCount);
    }
}

/* Documented in header. */
zcontext primitiveContext(void) {
    cstInit();

    zcontext ctx = langCtxNew();

    // These all could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    langCtxBind(ctx, "false", CST_FALSE);
    langCtxBind(ctx, "true", CST_TRUE);

    // Bind all the primitive functions.
    #define PRIM_FUNC(name) langCtxBindFunction(ctx, #name, prim_##name, NULL)
    #include "prim-def.h"

    // Include a binding for a maplet of all the bindings.
    langCtxBind(ctx, "PRIMLIB", langMapletFromCtx(ctx));

    return ctx;
}
