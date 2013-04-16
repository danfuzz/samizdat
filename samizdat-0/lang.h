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
 * Constant-related functions
 */

/**
 * Gets the in-model value typically known as `false`.
 */
zvalue langFalse(void);

/**
 * The value typically known as `true`.
 */
zvalue langTrue(void);

/**
 * Converts an in-model boolean value to a C `bool`.
 */
bool langBooleanToBool(zvalue value);

/**
 * Converts a C `bool` to an in-model boolean value.
 */
zvalue langBooleanFromBool(bool value);


/*
 * Functions about functions
 */

/**
 * Calls a value which is presumed to be bound as a function, passing
 * it the given listlet of arguments.
 */
zvalue langApply(zvalue functionId, zvalue args);

/**
 * Calls a value which is presumed to be bound as a function, passing
 * it the given number of arguments in the indicated array.
 */
zvalue langCall(zvalue functionId, zint argCount, const zvalue *args);

/**
 * Compile the given program text into an executable form, suitable
 * for passing to `langExecute()`. The text must be a program in
 * Samizdat Layer 0.
 */
zvalue langCompile(zvalue programText);


/*
 * Other functions
 */

/**
 * Constructs and returns a fresh execution context, valid for use as
 * an argument to `langExecute()` but without any initial variable
 * bindings.
 */
zcontext langCtxNew(void);

/**
 * Constructs and returns a fresh execution context, with the given
 * parent context.
 */
zcontext langCtxNewChild(zcontext parent);

/**
 * Binds an arbitrary value into the given execution context,
 * giving it the indicated name (interpreted as a `'\0'`-terminated
 * UTF-8 string).
 */
void langBind(zcontext ctx, const char *name, zvalue value);

/**
 * Binds a primitive function into the given execution context,
 * giving it the indicated name (interpreted as a `'\0'`-terminated
 * UTF-8 string). The given `state` will be passed as the first
 * argument to the function whenever it is called.
 */
void langBindFunction(zcontext ctx, const char *name,
                      zfunction function, void *state);

/**
 * Adds all of the bindings from the given maplet to the given
 * context.
 */
void langCtxBindAll(zcontext ctx, zvalue maplet);

/**
 * Calls the `main` function bound in the given context, passing
 * it the given number of arguments in the indicated array. This
 * returns whatever `main` returned, including possibly `NULL`
 * which indicates that the function did not actually return anything.
 */
zvalue langCallMain(zcontext ctx, zint argCount, const zvalue *args);

/**
 * Executes the given code, using the given global context.  Modifies
 * the context in response to running the code. In addition, this can
 * cause arbitrary calls to be performed on and via the context's
 * function registry (which can respond to those calls as it sees
 * fit).
 *
 * The given `code` must be a `block` node, such as would have
 * been returned from `langCompile()` on a Samizdat Layer 0 file.
 */
void langExecute(zcontext ctx, zvalue code);


#endif
