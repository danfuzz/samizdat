/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Prefab constants.
 */

#ifndef _CONSTS_H_
#define _CONSTS_H_

#include "sam-data.h"

/* Prefab stringlets. Lazily initialized. */
extern zvalue STR_CH_AT;
extern zvalue STR_CH_ATAT;
extern zvalue STR_CH_CARET;
extern zvalue STR_CH_CL_CURLY;
extern zvalue STR_CH_CL_PAREN;
extern zvalue STR_CH_CL_SQUARE;
extern zvalue STR_CH_COLON;
extern zvalue STR_CH_COLONCOLON;
extern zvalue STR_CH_EQUAL;
extern zvalue STR_CH_OP_CURLY;
extern zvalue STR_CH_OP_PAREN;
extern zvalue STR_CH_OP_SQUARE;
extern zvalue STR_CH_SEMICOLON;
extern zvalue STR_ACTUALS;
extern zvalue STR_BINDING;
extern zvalue STR_CALL;
extern zvalue STR_EXPRESSION;
extern zvalue STR_EXPRESSIONS;
extern zvalue STR_FORMALS;
extern zvalue STR_FUNCTION;
extern zvalue STR_IDENTIFIER;
extern zvalue STR_INTEGER;
extern zvalue STR_LISTLET;
extern zvalue STR_LITERAL;
extern zvalue STR_MAPLET;
extern zvalue STR_RETURN;
extern zvalue STR_STATEMENT;
extern zvalue STR_STATEMENTS;
extern zvalue STR_STRING;
extern zvalue STR_TYPE;
extern zvalue STR_UNIQLET;
extern zvalue STR_VALUE;
extern zvalue STR_VAR_DEF;
extern zvalue STR_VAR_REF;

/* Prefab tokens / nodes. Lazily initialized. */
extern zvalue TOK_CH_AT;
extern zvalue TOK_CH_ATAT;
extern zvalue TOK_CH_CARET;
extern zvalue TOK_CH_CL_CURLY;
extern zvalue TOK_CH_CL_PAREN;
extern zvalue TOK_CH_CL_SQUARE;
extern zvalue TOK_CH_COLON;
extern zvalue TOK_CH_COLONCOLON;
extern zvalue TOK_CH_EQUAL;
extern zvalue TOK_CH_OP_CURLY;
extern zvalue TOK_CH_OP_PAREN;
extern zvalue TOK_CH_OP_SQUARE;
extern zvalue TOK_CH_SEMICOLON;
extern zvalue TOK_ACTUALS;
extern zvalue TOK_BINDING;
extern zvalue TOK_CALL;
extern zvalue TOK_EXPRESSION;
extern zvalue TOK_EXPRESSIONS;
extern zvalue TOK_FORMALS;
extern zvalue TOK_FUNCTION;
extern zvalue TOK_IDENTIFIER;
extern zvalue TOK_INTEGER;
extern zvalue TOK_LISTLET;
extern zvalue TOK_LITERAL;
extern zvalue TOK_MAPLET;
extern zvalue TOK_RETURN;
extern zvalue TOK_STATEMENT;
extern zvalue TOK_STATEMENTS;
extern zvalue TOK_STRING;
extern zvalue TOK_UNIQLET;
extern zvalue TOK_VAR_DEF;
extern zvalue TOK_VAR_REF;

/**
 * Initializes the string and token variables, if necessary.
 */
void constsInit(void);

/**
 * Constructs a value token / node, by combining the given base
 * token with `value` mapping to the given value.
 */
zvalue valueToken(zvalue token, zvalue value);

#endif
