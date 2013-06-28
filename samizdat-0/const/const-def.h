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
TOK(CH_COMMA,      ",");
TOK(CH_DIAMOND,    "<>");
TOK(CH_DOT,        ".");
TOK(CH_DOTDOT,     "..");
TOK(CH_EQUAL,      "=");
TOK(CH_GT,         ">");
TOK(CH_LT,         "<");
TOK(CH_MINUS,      "-");
TOK(CH_OCURLY,     "{");
TOK(CH_OPAREN,     "(");
TOK(CH_OSQUARE,    "[");
TOK(CH_PARENPAREN, "()");
TOK(CH_PLUS,       "+");
TOK(CH_QMARK,      "?");
TOK(CH_SEMICOLON,  ";");
TOK(CH_STAR,       "*");
TOK(DEF,           "def");
TOK(FN,            "fn");
TOK(RETURN,        "return");

STR(UP_LIBRARY,           "LIBRARY");

STR(ACTUALS,              "actuals");
STR(BOOLEAN,              "boolean");
STR(CALL,                 "call");
STR(CLOSURE,              "closure");
STR(EMPTY,                ""); // The canonical empty string (`""`) value.
STR(EXPRESSION,           "expression");
STR(FALSE,                "false");
STR(FN_DEF,               "fnDef");
STR(FORMALS,              "formals");
STR(FUNCTION,             "function");
STR(IDENTIFIER,           "identifier");
STR(INEG,                 "ineg");
STR(INT,                  "int");
STR(INTERPOLATE,          "interpolate");
STR(LIST,                 "list");
STR(LITERAL,              "literal");
STR(MAKE_LIST,            "makeList");
STR(MAKE_MAP,             "makeMap");
STR(MAKE_RANGE_INCLUSIVE, "makeRangeInclusive");
STR(MAKE_TOKEN,           "makeToken");
STR(MAKE_UNIQLET,         "makeUniqlet");
STR(MAIN_SAM0,            "main.sam0");
STR(MAP,                  "map");
STR(NAME,                 "name");
STR(NONLOCAL_EXIT,        "nonlocalExit");
STR(REPEAT,               "repeat");
STR(RESULT,               "result");
STR(SAM_COMMAND_LINE,     "samCommandLine");
STR(STATE,                "state");
STR(STATEMENTS,           "statements");
STR(STRING,               "string");
STR(TOKEN,                "token");
STR(TRUE,                 "true");
STR(TYPE,                 "type");
STR(UNIQLET,              "uniqlet");
STR(VALUE,                "value");
STR(VAR_DEF,              "varDef");
STR(VAR_REF,              "varRef");
STR(YIELD,                "yield");
STR(YIELD_DEF,            "yieldDef");
