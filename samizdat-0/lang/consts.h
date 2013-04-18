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
extern zvalue STR_CH_CCURLY;
extern zvalue STR_CH_CPAREN;
extern zvalue STR_CH_CSQUARE;
extern zvalue STR_CH_COLON;
extern zvalue STR_CH_COLONCOLON;
extern zvalue STR_CH_DIAMOND;
extern zvalue STR_CH_DOT;
extern zvalue STR_CH_EQUAL;
extern zvalue STR_CH_OCURLY;
extern zvalue STR_CH_OPAREN;
extern zvalue STR_CH_OSQUARE;
extern zvalue STR_CH_SEMICOLON;
extern zvalue STR_CH_STAR;
extern zvalue STR_ACTUALS;
extern zvalue STR_BLOCK;
extern zvalue STR_BOOLEAN;
extern zvalue STR_CALL;
extern zvalue STR_FORMALS;
extern zvalue STR_FUNCTION;
extern zvalue STR_HIGHLET;
extern zvalue STR_IDENTIFIER;
extern zvalue STR_INTEGER;
extern zvalue STR_KEY;
extern zvalue STR_LISTLET;
extern zvalue STR_LITERAL;
extern zvalue STR_MAPLET;
extern zvalue STR_NAME;
extern zvalue STR_REPEAT;
extern zvalue STR_STATEMENTS;
extern zvalue STR_STRING;
extern zvalue STR_TYPE;
extern zvalue STR_UNIQLET;
extern zvalue STR_VALUE;
extern zvalue STR_VAR_DEF;
extern zvalue STR_VAR_REF;
extern zvalue STR_YIELD;

/* Prefab tokens / nodes. Lazily initialized. */
extern zvalue TOK_CH_AT;
extern zvalue TOK_CH_ATAT;
extern zvalue TOK_CH_CCURLY;
extern zvalue TOK_CH_CPAREN;
extern zvalue TOK_CH_CSQUARE;
extern zvalue TOK_CH_COLON;
extern zvalue TOK_CH_COLONCOLON;
extern zvalue TOK_CH_DIAMOND;
extern zvalue TOK_CH_DOT;
extern zvalue TOK_CH_EQUAL;
extern zvalue TOK_CH_OCURLY;
extern zvalue TOK_CH_OPAREN;
extern zvalue TOK_CH_OSQUARE;
extern zvalue TOK_CH_SEMICOLON;
extern zvalue TOK_CH_STAR;
extern zvalue TOK_UNIQLET;

/**
 * Initializes the string and token variables, if necessary.
 */
void constsInit(void);

#endif
