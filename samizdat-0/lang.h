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
typedef zvalue (*zfunction)(zvalue state, zint argCount, const zvalue *args);


/*
 * Function definition and application
 */

/**
 * Calls a value which is presumed to be bound as a function, passing
 * it the given list of arguments.
 */
zvalue langApply(zvalue functionId, zvalue args);

/**
 * Calls a value which is presumed to be bound as a function, passing
 * it the given number of arguments in the indicated array.
 */
zvalue langCall(zvalue functionId, zint argCount, const zvalue *args);

/**
 * Defines a function with associated (and arbitrary) closure state and
 * optional name (used when producing stack traces). Returns the identifying
 * uniqlet that binds to it.
 */
zvalue langDefineFunction(zfunction function, zvalue state, zvalue name);


/*
 * Compilation
 */

/**
 * Evaluates the given expression node in the given variable
 * context. Returns the evaluated value of the expression, which
 * will be `NULL` if the expression did not yield a value.
 *
 * See the *Samizdat Layer 0* spec for details on expression nodes.
 */
zvalue langEval0(zvalue ctx, zvalue node);

/**
 * Tokenizes the given program text into a list of tokens, suitable
 * for passing to `langTree0()`.
 *
 * See the *Samizdat Layer 0* spec for details about the grammar.
 */
zvalue langTokenize0(zvalue programText);

/**
 * Compiles the given program text into a parse tree form, suitable
 * for passing to `langEval0()`. `program` must either
 * be a string or a list of tokens, and it must represent a top-level
 * program in Samizdat Layer 0. The result is a `function` node in the
 * *Samizdat Layer 0* parse tree form.
 *
 * See the *Samizdat Layer 0* spec for details about the grammar.
 */
zvalue langTree0(zvalue program);


/*
 * Boxes
 */

/**
 * Gets the value out of the given box. Returns `NULL` if the box is
 * void or as yet unset.
 */
zvalue boxGet(zvalue boxUniqlet);

/**
 * Returns an indication of whether or not the given box has been set.
 */
bool boxIsSet(zvalue boxUniqlet);

/**
 * Resets the given box to an un-set state. The box must be a mutable box.
 */
void boxReset(zvalue boxUniqlet);

/**
 * Sets the value of the given box as indicated. Passing `value` as
 * `NULL` indicates that the box is to be set to void.
 */
void boxSet(zvalue boxUniqlet, zvalue value);

/**
 * Constructs a mutable (re-settable) box.
 */
zvalue boxMutable(void);

/**
 * Constructs a yield (set-once) box.
 */
zvalue boxYield(void);

#endif
