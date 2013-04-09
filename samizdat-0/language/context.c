/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"


/*
 * Module functions
 */

zcontext ctxNewEmpty(void) {
    return zalloc(sizeof(ExecutionContext));
}

zcontext ctxNewChild(zcontext parent, zvalue locals) {
    zcontext ctx = ctxNewEmpty();

    ctx->locals = locals;
    ctx->parent = parent;
    ctx->reg = parent->reg;

    return ctx;
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext ctxNewTopLevel(void) {
    zcontext ctx = ctxNewEmpty();

    ctx->locals = samMapletEmpty();
    ctx->reg = funNew();

    return ctx;
}
