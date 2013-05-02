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
zvalue langEvalExpressionNode(zvalue ctx, zvalue node);


#endif
