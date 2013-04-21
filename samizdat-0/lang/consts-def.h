/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Note: This file gets `#include`d multiple times, and so does not
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
TOK(CH_OCURLY,     "{");
TOK(CH_OPAREN,     "(");
TOK(CH_OSQUARE,    "[");
TOK(CH_SEMICOLON,  ";");
TOK(CH_STAR,       "*");
STR(ACTUALS,       "actuals");
STR(BOOLEAN,       "boolean");
STR(CALL,          "call");
STR(FORMALS,       "formals");
STR(FUNCTION,      "function");
STR(HIGHLET,       "highlet");
STR(IDENTIFIER,    "identifier");
STR(INTEGER,       "integer");
STR(INTLET,        "intlet");
STR(LISTLET,       "listlet");
STR(LITERAL,       "literal");
STR(MAKE_HIGHLET,  "makeHighlet");
STR(MAKE_LISTLET,  "makeListlet");
STR(MAKE_MAPLET,   "makeMaplet");
STR(MAKE_UNIQLET,  "makeUniqlet");
STR(MAPLET,        "maplet");
STR(NAME,          "name");
STR(REPEAT,        "repeat");
STR(STATEMENTS,    "statements");
STR(STRING,        "string");
STR(STRINGLET,     "stringlet");
STR(TYPE,          "type");
STR(UNIQLET,       "uniqlet");
STR(VALUE,         "value");
STR(VAR_DEF,       "varDef");
STR(VAR_REF,       "varRef");
STR(YIELD,         "yield");
