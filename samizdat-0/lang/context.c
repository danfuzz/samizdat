/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/* Documented in header. */
typedef struct ExecutionContext {
    /** Variables bound at this level. */
    zvalue locals;

    /** Variables bound in the parent. */
    zvalue parent;
} ExecutionContext;


/*
 * Module functions
 */

/* Documented in header. */
zcontext ctxNewChild(zvalue parent, zvalue locals) {
    zcontext ctx = zalloc(sizeof(ExecutionContext));

    ctx->locals = locals;
    ctx->parent = parent;

    return ctx;
}

/* Documented in header. */
void ctxBind(zcontext ctx, zvalue name, zvalue value) {
    if (datMapletGet(ctx->locals, name) != NULL) {
        die("Duplicate assignment.");
    }

    ctx->locals = datMapletPut(ctx->locals, name, value);
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext langCtxNew(void) {
    return ctxNewChild(EMPTY_MAPLET, EMPTY_MAPLET);
}

/* Documented in header. */
void langCtxBind(zcontext ctx, zvalue name, zvalue value) {
    ctxBind(ctx, name, value);
}

/* Documented in header. */
void langCtxBindFunction(zcontext ctx, const char *name,
                         zfunction function, void *state) {
    langCtxBind(ctx,
        datStringletFromUtf8(-1, name),
        langDefineFunction(function, state));
}

/* Documented in header. */
zvalue langMapletFromCtx(zcontext ctx) {
    return datMapletAdd(ctx->parent, ctx->locals);
}
