/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Samizdat Layer 0 language implementation
 */

#ifndef _LANG_H_
#define _LANG_H_

#include "dat.h"


/*
 * Data types
 */

/**
 * Execution context. This contains global variable bindings and
 * definitions / bindings of functions (both primitive and
 * in-language).  The contents of a context are *not* directly
 * accessible through instances of this type via the API. You have to
 * use the various accessor functions.
 */
typedef struct ExecutionContext *zcontext;

/**
 * Prototype for all functions bound into execution contexts. The
 * `state` is arbitrary closure state (passed in when the function
 * was bound).
 */
typedef zvalue (*zfunction)(void *state, zint argCount, const zvalue *args);


/*
 * Functions
 */

/**
 * Gets the in-model value typically known as `null`.
 */
zvalue langNull(void);

/**
 * Gets the in-model value typically known as `false`.
 */
zvalue langFalse(void);

/**
 * The value typically known as `true`.
 */
zvalue langTrue(void);

/**
 * Constructs and returns a fresh execution context, valid for use as
 * an argument to `langExecute()` but without any initial variable
 * bindings.
 */
zcontext langNewContext(void);

/**
 * Binds a primitive function into the given execution context,
 * giving it the indicated name (interpreted as a `'\0'`-terminated
 * UTF-8 string). The given `state` will be passed as the first
 * argument to the function whenever it is called.
 */
void langBindFunction(zcontext ctx, const char *name,
                      zfunction function, void *state);

/**
 * Executes the given code, using the given global context.  Modifies
 * the context in response to running the code.  In addition, this can
 * cause arbitrary calls to be performed on and via the context's
 * function registry (which can respond to those calls as it sees
 * fit).
 *
 * The given `code` must be a `statements` node, such as would have
 * been returned from `parse()` on a Samizdat Layer 0 file.
 */
void langExecute(zcontext ctx, zvalue code);

/**
 * Compile the given program text into an executable form, suitable
 * for passing to `langExecute()`. The text must be a program in
 * Samizdat Layer 0.
 */
zvalue langCompile(zvalue programText);

#endif
