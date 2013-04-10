/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "io.h"
#include "impl.h"
#include "lib.h"
#include "util.h"

#include <stddef.h>


/*
 * Primitive implementations (exported via the context)
 */

/**
 * TODO: Document!
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

/**
 * TODO: Document!
 */
static zvalue prim_readFile(void *state, zint argCount, const zvalue *args) {
    if (argCount != 1) {
        die("Invalid argument count to readFile: %lld", argCount);
    }

    return readFile(args[0]);
}

/**
 * TODO: Document!
 */
static zvalue prim_writeFile(void *state, zint argCount, const zvalue *args) {
    if (argCount != 2) {
        die("Invalid argument count to writeFile: %lld", argCount);
    }

    writeFile(args[0], args[1]);
    return langNull();
}


/*
 * Module functions
 */

/* Documented in header. */
void bindPrimitives(zcontext ctx) {
    langBindFunction(ctx, "if",        prim_if,        NULL);
    langBindFunction(ctx, "readFile",  prim_readFile,  NULL);
    langBindFunction(ctx, "writeFile", prim_writeFile, NULL);
    // TODO: More.
}
