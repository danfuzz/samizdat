/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
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
 * Function definition and application
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
 * Defines a function with associated (and arbitrary) closure
 * state. Returns the identifying uniqlet that binds to it.
 */
zvalue langDefineFunction(zfunction function, void *state);


/*
 * Execution contexts
 */

/**
 * Constructs and returns a fresh execution context, valid for use as
 * an argument to `langEvalExpressionNode()` but without any variable
 * bindings in either as parent bindings or local variables.
 */
zcontext langCtxNew(void);

/**
 * Constructs and returns a fresh execution context, with the given
 * parent context but with no initial local variable bindings.
 */
zcontext langCtxNewChild(zcontext parent);

/**
 * Binds an arbitrary value into the given execution context,
 * giving it the indicated name (interpreted as a `'\0'`-terminated
 * UTF-8 string).
 */
void langCtxBind(zcontext ctx, zvalue name, zvalue value);

/**
 * Binds a primitive function into the given execution context,
 * giving it the indicated name (interpreted as a `'\0'`-terminated
 * UTF-8 string). The given `state` will be passed as the first
 * argument to the function whenever it is called.
 */
void langCtxBindFunction(zcontext ctx, const char *name,
                         zfunction function, void *state);

/**
 * Adds all of the bindings from the given maplet to the given
 * context.
 */
void langCtxBindAll(zcontext ctx, zvalue maplet);

/**
 * Gets a maplet of all the bindings in the given context. Note
 * that this is a snapshot; any subsequent modification to the
 * bindings will have no effect on the result returned from this
 * function.
 */
zvalue langMapletFromCtx(zcontext ctx);


/*
 * Compilation
 */

/**
 * Compiles the given program text into a parse tree form, suitable
 * for passing to `langEvalExpressionNode()`. The text must be a
 * top-level program in Samizdat Layer 0. The result is a `function`
 * node in the *Samizdat Layer 0* parse tree form.
 *
 * See the *Samizdat Layer 0* spec for details about the grammar.
 */
zvalue langNodeFromProgramText(zvalue programText);

/**
 * Evaluates the given expression node in the given variable
 * context. Returns the evaluated value of the expression, which
 * will be `NULL` if the expression did not yield a value.
 *
 * See the *Samizdat Layer 0* spec for details on expression nodes.
 */
zvalue langEvalExpressionNode(zcontext ctx, zvalue node);


#endif
