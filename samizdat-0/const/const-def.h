/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * **Note:** This file gets `#include`d multiple times, and so does not
 * have the usual guard macros.
 *
 * `STR(name, "string")` defines a string constant.
 * `TOK(name, "string")` defines both a string *and* a token constant.
 */

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
TOK(CH_GT,         ">");
TOK(CH_LT,         "<");
TOK(CH_MINUS,      "-");
TOK(CH_OCURLY,     "{");
TOK(CH_OPAREN,     "(");
TOK(CH_OSQUARE,    "[");
TOK(CH_QMARK,      "?");
TOK(CH_SEMICOLON,  ";");
TOK(CH_STAR,       "*");

STR(CH_DOTDOT,     "..");
STR(CH_SLASH,      "/");

STR(UP_LIBRARY,    "LIBRARY");

STR(ACTUALS,       "actuals");
STR(BOOLEAN,       "boolean");
STR(CALL,          "call");
STR(EMPTY,         ""); // The canonical empty string (`@""`) value.
STR(FALSE,         "false");
STR(FORMALS,       "formals");
STR(FUNCTION,      "function");
STR(HIGHLET,       "highlet");
STR(IDENTIFIER,    "identifier");
STR(INTEGER,       "integer");
STR(LIST,       "list");
STR(LITERAL,       "literal");
STR(MAKE_HIGHLET,  "makeHighlet");
STR(MAKE_LIST,  "makeList");
STR(MAKE_MAPLET,   "makeMap");
STR(MAKE_UNIQLET,  "makeUniqlet");
STR(MAIN,          "main");
STR(MAPLET,        "map");
STR(NAME,          "name");
STR(REPEAT,        "repeat");
STR(RESULT,        "result");
STR(STATE,         "state");
STR(STATEMENTS,    "statements");
STR(STRING,        "string");
STR(TRUE,          "true");
STR(TYPE,          "type");
STR(UNIQLET,       "uniqlet");
STR(VALUE,         "value");
STR(VAR_DEF,       "varDef");
STR(VAR_REF,       "varRef");
STR(YIELD,         "yield");
STR(YIELD_DEF,     "yieldDef");
