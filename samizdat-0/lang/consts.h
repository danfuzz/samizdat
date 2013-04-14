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

#include "dat.h"

/* Prefab stringlets. Lazily initialized. */
extern zvalue STR_CH_AT;
extern zvalue STR_CH_ATAT;
extern zvalue STR_CH_CARET;
extern zvalue STR_CH_CCURLY;
extern zvalue STR_CH_CPAREN;
extern zvalue STR_CH_CSQUARE;
extern zvalue STR_CH_COLONCOLON;
extern zvalue STR_CH_DOT;
extern zvalue STR_CH_EQUAL;
extern zvalue STR_CH_OCURLY;
extern zvalue STR_CH_OPAREN;
extern zvalue STR_CH_OSQUARE;
extern zvalue STR_CH_SEMICOLON;
extern zvalue STR_CH_STAR;
extern zvalue STR_ACTUALS;
extern zvalue STR_BOOLEAN;
extern zvalue STR_CALL;
extern zvalue STR_FORMALS;
extern zvalue STR_FUNCTION;
extern zvalue STR_IDENTIFIER;
extern zvalue STR_INTEGER;
extern zvalue STR_KEY;
extern zvalue STR_LISTLET;
extern zvalue STR_LITERAL;
extern zvalue STR_MAIN;
extern zvalue STR_MAPLET;
extern zvalue STR_NAME;
extern zvalue STR_NULL;
extern zvalue STR_REPEAT;
extern zvalue STR_RETURN;
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
extern zvalue TOK_CH_CCURLY;
extern zvalue TOK_CH_CPAREN;
extern zvalue TOK_CH_CSQUARE;
extern zvalue TOK_CH_COLONCOLON;
extern zvalue TOK_CH_DOT;
extern zvalue TOK_CH_EQUAL;
extern zvalue TOK_CH_OCURLY;
extern zvalue TOK_CH_OPAREN;
extern zvalue TOK_CH_OSQUARE;
extern zvalue TOK_CH_SEMICOLON;
extern zvalue TOK_CH_STAR;
extern zvalue TOK_ACTUALS;
extern zvalue TOK_BOOLEAN;
extern zvalue TOK_CALL;
extern zvalue TOK_FORMALS;
extern zvalue TOK_FUNCTION;
extern zvalue TOK_IDENTIFIER;
extern zvalue TOK_INTEGER;
extern zvalue TOK_LISTLET;
extern zvalue TOK_LITERAL;
extern zvalue TOK_MAPLET;
extern zvalue TOK_NULL;
extern zvalue TOK_RETURN;
extern zvalue TOK_STATEMENTS;
extern zvalue TOK_STRING;
extern zvalue TOK_UNIQLET;
extern zvalue TOK_VAR_DEF;
extern zvalue TOK_VAR_REF;

/**
 * Initializes the string and token variables, if necessary.
 */
void constsInit(void);

#endif
