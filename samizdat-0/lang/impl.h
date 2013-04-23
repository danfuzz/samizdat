/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Private structure implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lang.h"


/*
 * Execution Contexts
 */

/* Documented in header. */
typedef struct ExecutionContext {
    /** Variables bound at this level. */
    zvalue locals;

    /** Parent context. */
    struct ExecutionContext *parent;
} ExecutionContext;

/**
 * Allocates an execution context set up to be the child of the given
 * one and with the given initial locals.
 */
zcontext ctxNewChild(zcontext parent, zvalue locals);

/**
 * Binds a new variable in the given context.
 */
void ctxBind(zcontext ctx, zvalue name, zvalue value);

/**
 * Gets the value of the variable bound in the given context, either
 * directly or indirectly via its parent chain. It is an error if
 * the name is not bound.
 */
zvalue ctxGet(zcontext ctx, zvalue name);


/*
 * Other
 */

/**
 * Defines a function with associated (and arbitrary) closure
 * state. Returns the identifying uniqlet that binds to it.
 */
zvalue funDefine(zfunction function, void *state);

/**
 * Tokenizes a stringlet using Samizdat Layer 0 token syntax. Returns
 * a listlet of tokens.
 */
zvalue tokenize(zvalue stringlet);


#endif
