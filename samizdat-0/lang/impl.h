/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Private structure implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lang.h"


/*
 * Data types
 */

/**
 * Function registry. The contents of a registry are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct FunctionRegistry *zfunreg;

/* Documented in header. */
typedef struct ExecutionContext {
    /** Variables bound at this level. */
    zvalue locals;

    /** Pending return value. */
    zvalue toReturn;

    /** Parent context. */
    struct ExecutionContext *parent;

    /** Function registry. */
    zfunreg reg;
} ExecutionContext;


/*
 * Execution contexts
 */

/**
 * Allocates a fresh empty context.
 */
zcontext ctxNewEmpty(void);

/**
 * Allocates a context set up to be the child of the given one and
 * with the given initial locals.
 */
zcontext ctxNewChild(zcontext parent, zvalue locals);


/*
 * Function registries
 */

/**
 * Prototype of all bound functions.
 */
typedef zvalue (*zfunction)(void *state, zint argCount, const zvalue *args);

/**
 * Constructs and returns a new function registry.
 */
zfunreg funNew(void);

/**
 * Adds a function with associated (and arbitrary) closure
 * state. Returns the identifying uniqlet that binds to it.
 */
zvalue funAdd(zfunreg reg, zfunction function, void *state);

/**
 * Calls the function bound to the given uniqlet.
 */
zvalue funCall(zfunreg reg, zvalue id, zint argCount, const zvalue *args);


/*
 * High-Layer data model
 */

/**
 * Constructs a value token / node, by combining the given base
 * token with `value` binding to the given value.
 */
zvalue valueToken(zvalue token, zvalue value);

/**
 * Gets the `type` binding of the given maplet (*not* the low layer
 * Samizdat type).
 */
zvalue hidType(zvalue value);

/**
 * Gets the `value` binding of the given maplet.
 */
zvalue hidValue(zvalue value);

/**
 * Returns whether or not the `type` binding of the given maplet
 * equals the given value.
 */
bool hidHasType(zvalue value, zvalue type);

/**
 * Asserts that the given value is a maplet whose type is as given.
 */
void hidAssertType(zvalue value, zvalue type);


/*
 * Other
 */

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a stringlet (listlet of Unicode-representing
 * intlets) of the contents.
 */
zvalue readFile(zvalue fileName);

/**
 * Tokenizes a stringlet using Samizdat Layer 0 token syntax. Returns
 * a listlet of tokens.
 */
zvalue tokenize(zvalue stringlet);

/**
 * Parses a listlet of tokens into Samizdat Layer 0 parse trees. Returns
 * a node representing the parsed program.
 */
zvalue parse(zvalue tokens);


#endif
