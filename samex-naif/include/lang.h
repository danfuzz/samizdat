// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Samizdat Layer 0 language implementation
//

#ifndef _LANG_H_
#define _LANG_H_

#include "dat.h"


//
// Compilation
//

/**
 * Evaluates the given expression node in the given variable
 * environment. Returns the evaluated value of the expression, which
 * will be `NULL` if the expression evaluated to void (only possible when
 * given a `@maybe` node). `env` must be a symbol table.
 *
 * See the spec for details on expression nodes.
 */
zvalue langEval0(zvalue env, zvalue node);

/**
 * Gets the language directive in the given program text, if any.
 *
 * See the spec for details on language directives.
 */
zvalue langLanguageOf0(zvalue programText);

/**
 * Compiles the given expression text into a parse tree form, suitable
 * for passing to `langSimplify0()`. `expression` must either
 * be a string or a list of tokens, and it must represent an expression
 * in Samizdat Layer 0. The result is an expression node in the
 * Samizdat Layer 0 parse tree form.
 *
 * See the spec for details about the grammar.
 */
zvalue langParseExpression0(zvalue expression);

/**
 * Compiles the given program text into a parse tree form, suitable
 * for passing to `langSimplify0()`. `program` must either
 * be a string or a list of tokens, and it must represent a top-level
 * program in Samizdat Layer 0. The result is a `function` node in the
 * Samizdat Layer 0 parse tree form.
 *
 * See the spec for details about the grammar.
 */
zvalue langParseProgram0(zvalue program);

/**
 * Simplifies the given expression node to a form that is suitable for
 * passing to `langEval0`. `resolveFn` is allowed to be `NULL`.
 *
 * See the spec for details about the grammar.
 */
zvalue langSimplify0(zvalue node, zvalue resolveFn);

/**
 * Tokenizes the given program text into a list of tokens, suitable
 * for passing to `langParseProgram0()`.
 *
 * See the spec for details about the grammar.
 */
zvalue langTokenize0(zvalue programText);


#endif
