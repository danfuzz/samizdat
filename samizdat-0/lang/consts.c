/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"

#include <stddef.h>

zvalue STR_CH_AT = NULL;
zvalue STR_CH_ATAT = NULL;
zvalue STR_CH_CCURLY = NULL;
zvalue STR_CH_CPAREN = NULL;
zvalue STR_CH_CSQUARE = NULL;
zvalue STR_CH_COLON = NULL;
zvalue STR_CH_COLONCOLON = NULL;
zvalue STR_CH_DIAMOND = NULL;
zvalue STR_CH_DOT = NULL;
zvalue STR_CH_EQUAL = NULL;
zvalue STR_CH_OCURLY = NULL;
zvalue STR_CH_OPAREN = NULL;
zvalue STR_CH_OSQUARE = NULL;
zvalue STR_CH_SEMICOLON = NULL;
zvalue STR_CH_STAR = NULL;
zvalue STR_ACTUALS = NULL;
zvalue STR_BLOCK = NULL;
zvalue STR_BOOLEAN = NULL;
zvalue STR_CALL = NULL;
zvalue STR_FORMALS = NULL;
zvalue STR_FUNCTION = NULL;
zvalue STR_HIGHLET = NULL;
zvalue STR_IDENTIFIER = NULL;
zvalue STR_INTEGER = NULL;
zvalue STR_KEY = NULL;
zvalue STR_LISTLET = NULL;
zvalue STR_LITERAL = NULL;
zvalue STR_MAPLET = NULL;
zvalue STR_NAME = NULL;
zvalue STR_REPEAT = NULL;
zvalue STR_STATEMENTS = NULL;
zvalue STR_STRING = NULL;
zvalue STR_TYPE = NULL;
zvalue STR_UNIQLET = NULL;
zvalue STR_VALUE = NULL;
zvalue STR_VAR_DEF = NULL;
zvalue STR_VAR_REF = NULL;
zvalue STR_YIELD = NULL;

/* Prefab tokens / nodes. Lazily initialized. */
zvalue TOK_CH_AT = NULL;
zvalue TOK_CH_ATAT = NULL;
zvalue TOK_CH_CCURLY = NULL;
zvalue TOK_CH_CPAREN = NULL;
zvalue TOK_CH_CSQUARE = NULL;
zvalue TOK_CH_COLON = NULL;
zvalue TOK_CH_COLONCOLON = NULL;
zvalue TOK_CH_DIAMOND = NULL;
zvalue TOK_CH_DOT = NULL;
zvalue TOK_CH_EQUAL = NULL;
zvalue TOK_CH_OCURLY = NULL;
zvalue TOK_CH_OPAREN = NULL;
zvalue TOK_CH_OSQUARE = NULL;
zvalue TOK_CH_SEMICOLON = NULL;
zvalue TOK_CH_STAR = NULL;
zvalue TOK_UNIQLET = NULL;


/*
 * Module functions
 */

/* Documented in header. */
void constsInit(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    #define STR(name, str) STR_##name = datStringletFromUtf8String(str, -1)
    #define TOK(name, str) \
        STR(name, str); \
        TOK_##name = datHighletFromType(STR_##name);

    TOK(CH_AT,         "@");
    TOK(CH_ATAT,       "@@");
    TOK(CH_CCURLY,     "}");
    TOK(CH_CPAREN,     ")");
    TOK(CH_CSQUARE,    "]");
    TOK(CH_COLON,      ":");
    TOK(CH_COLONCOLON, "::");
    TOK(CH_DIAMOND,    "<>");
    TOK(CH_DOT,        ".");
    TOK(CH_EQUAL,      "=");
    TOK(CH_OCURLY,     "{");
    TOK(CH_OPAREN,     "(");
    TOK(CH_OSQUARE,    "[");
    TOK(CH_SEMICOLON,  ";");
    TOK(CH_STAR,       "*");
    STR(ACTUALS,       "actuals");
    STR(BLOCK,         "block");
    STR(BOOLEAN,       "boolean");
    STR(CALL,          "call");
    STR(FORMALS,       "formals");
    STR(FUNCTION,      "function");
    STR(HIGHLET,       "highlet");
    STR(IDENTIFIER,    "identifier");
    STR(INTEGER,       "integer");
    STR(KEY,           "key");
    STR(LISTLET,       "listlet");
    STR(LITERAL,       "literal");
    STR(MAPLET,        "maplet");
    STR(NAME,          "name");
    STR(REPEAT,        "repeat");
    STR(STATEMENTS,    "statements");
    STR(STRING,        "string");
    STR(TYPE,          "type");
    TOK(UNIQLET,       "uniqlet");
    STR(VALUE,         "value");
    STR(VAR_DEF,       "varDef");
    STR(VAR_REF,       "varRef");
    STR(YIELD,         "yield");
}
