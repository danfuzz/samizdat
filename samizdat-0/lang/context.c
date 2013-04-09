/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Module functions
 */

zcontext ctxNewChild(zcontext parent, zvalue locals) {
    zcontext ctx = zalloc(sizeof(ExecutionContext));

    ctx->locals = locals;
    ctx->parent = parent;

    return ctx;
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext langNewContext(void) {
    return ctxNewChild(NULL, datMapletEmpty());
}
