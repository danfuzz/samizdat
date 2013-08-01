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

TOK(CAP_NULL_BOX,  "NullBox");

TOK(DEF,           "def");
TOK(FN,            "fn");
TOK(RETURN,        "return");

STR(EMPTY,            ""); // The canonical empty string (`""`) value.

STR(UP_LIBRARY,       "LIBRARY");

STR(ACTUALS,          "actuals");
STR(CALL,             "call");
STR(CLOSURE,          "closure");
STR(EXPRESSION,       "expression");
STR(FALSE,            "false");
STR(FN_DEF,           "fnDef");
STR(FORMALS,          "formals");
STR(FUNCTION,         "function");
STR(IDENTIFIER,       "identifier");
STR(INEG,             "ineg");
STR(INT,              "int");
STR(INTERPOLATE,      "interpolate");
STR(LITERAL,          "literal");
STR(MAKE_LIST,        "makeList");
STR(MAKE_MAPPING,     "makeMapping");
STR(MAKE_UNIQLET,     "makeUniqlet");
STR(MAKE_VALUE,       "makeValue");
STR(MAIN_SAM0,        "main.sam0");
STR(MAP_ADD,          "mapAdd");
STR(NAME,             "name");
STR(NONLOCAL_EXIT,    "nonlocalExit");
STR(REPEAT,           "repeat");
STR(RESULT,           "result");
STR(SAM_COMMAND_LINE, "samCommandLine");
STR(STATE,            "state");
STR(STATEMENTS,       "statements");
STR(STRING,           "string");
STR(TRUE,             "true");
STR(TYPE,             "type");
STR(VALUE,            "value");
STR(VAR_DEF,          "varDef");
STR(VAR_REF,          "varRef");
STR(VOIDABLE,         "voidable");
STR(YIELD,            "yield");
STR(YIELD_DEF,        "yieldDef");
