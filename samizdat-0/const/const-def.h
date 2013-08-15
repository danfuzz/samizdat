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
 *
 * Token constants are type-only values whose types are the indicated strings.
 */

TOK(CH_AND,        "&");
TOK(CH_AT,         "@");
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
TOK(CH_PLUS,       "+");
TOK(CH_QMARK,      "?");
TOK(CH_SEMICOLON,  ";");
TOK(CH_STAR,       "*");

TOK(DEF,           "def");
TOK(FN,            "fn");
TOK(RETURN,        "return");

STR(LIBRARY,       "LIBRARY");

STR(actuals,          "actuals");
STR(call,             "call");
STR(closure,          "closure");
STR(expression,       "expression");
STR(fnDef,            "fnDef");
STR(formals,          "formals");
STR(function,         "function");
STR(identifier,       "identifier");
STR(ineg,             "ineg");
STR(INT,              "int");
STR(interpolate,      "interpolate");
STR(literal,          "literal");
STR(makeList,         "makeList");
STR(makeMapping,      "makeMapping");
STR(makeValue,        "makeValue");
STR(main_sam0,        "main.sam0");
STR(mapAdd,           "mapAdd");
STR(name,             "name");
STR(nonlocalExit,     "nonlocalExit");
STR(optValue,         "optValue");
STR(repeat,           "repeat");
STR(samCommandLine,   "samCommandLine");
STR(statements,       "statements");
STR(string,           "string");
STR(TYPE,             "type");
STR(VALUE,            "value");
STR(varDef,           "varDef");
STR(varRef,           "varRef");
STR(voidable,         "voidable");
STR(yield,            "yield");
STR(yieldDef,         "yieldDef");
