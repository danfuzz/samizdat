/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * **Note:** This file gets `#include`d multiple times, and so does not
 * have the usual guard macros.
 *
 * `STR(name, "string")` defines a string constant.
 * `TYP(name, "string")` defines a string and a derived data type.
 * `TOK(name, "string")` defines a string, a type, and a token constant.
 *
 * Token constants are type-only transparent derived values, whose types are
 * the indicated strings.
 */

TOK(CH_AT,                 "@");
TOK(CH_ATAT,               "@@");
TOK(CH_CCURLY,             "}");
TOK(CH_CPAREN,             ")");
TOK(CH_CSQUARE,            "]");
TOK(CH_COLON,              ":");
TOK(CH_COLONCOLON,         "::");
TOK(CH_COLONEQUAL,         ":=");
TOK(CH_COMMA,              ",");
TOK(CH_DIAMOND,            "<>");
TOK(CH_DOLLAR,             "$");
TOK(CH_DOT,                ".");
TOK(CH_DOTDOT,             "..");
TOK(CH_EQUAL,              "=");
TOK(CH_GT,                 ">");
TOK(CH_LT,                 "<");
TOK(CH_MINUS,              "-");
TOK(CH_OCURLY,             "{");
TOK(CH_OPAREN,             "(");
TOK(CH_OSQUARE,            "[");
TOK(CH_PLUS,               "+");
TOK(CH_QMARK,              "?");
TOK(CH_RARROW,             "->");
TOK(CH_SEMICOLON,          ";");
TOK(CH_SLASH,              "/");
TOK(CH_STAR,               "*");
TOK(break,                 "break");
TOK(continue,              "continue");
TOK(def,                   "def");
TOK(export,                "export");
TOK(fn,                    "fn");
TOK(import,                "import");
TOK(return,                "return");
TOK(var,                   "var");

TYP(MapGenerator,          "MapGenerator");
TYP(SequenceGenerator,     "SequenceGenerator");
TYP(apply,                 "apply");
TYP(call,                  "call");
TYP(closure,               "closure");
TYP(directive,             "directive");
TYP(exportSelection,       "exportSelection");
TYP(external,              "external");
TYP(identifier,            "identifier");
TYP(importModule,          "importModule");
TYP(importModuleSelection, "importModuleSelection");
TYP(importResource,        "importResource");
TYP(int,                   "int");
TYP(internal,              "internal");
TYP(jump,                  "jump");
TYP(literal,               "literal");
TYP(string,                "string");
TYP(varBind,               "varBind");
TYP(varDef,                "varDef");
TYP(varDefMutable,         "varDefMutable");
TYP(varRef,                "varRef");

STR(actuals,               "actuals");
STR(bind,                  "bind");
STR(cat,                   "cat");
STR(collect,               "collect");
STR(directory,             "directory");
STR(file,                  "file");
STR(formals,               "formals");
STR(format,                "format");
STR(function,              "function");
STR(genericBind,           "genericBind");
STR(get,                   "get");
STR(index,                 "index");
STR(interpolate,           "interpolate");
STR(intraLoad,             "intraLoad");
STR(intraRead,             "intraRead");
STR(language,              "language");
STR(main,                  "main");
STR(makeDerivedDataType,   "makeDerivedDataType");
STR(makeList,              "makeList");
STR(makeRegularGeneric,    "makeRegularGeneric");
STR(makeUnitypeGeneric,    "makeUnitypeGeneric");
STR(makeValue,             "makeValue");
STR(makeValueMap,          "makeValueMap");
STR(map,                   "map");
STR(moduleLoad,            "moduleLoad");
STR(name,                  "name");
STR(optValue,              "optValue");
STR(other,                 "other");
STR(prefix,                "prefix");
STR(repeat,                "repeat");
STR(runCommandLine,        "runCommandLine");
STR(select,                "select");
STR(seq,                   "seq");
STR(source,                "source");
STR(statements,            "statements");
STR(this,                  "this");
STR(top,                   "top");
STR(type,                  "type");
STR(value,                 "value");
STR(yield,                 "yield");
STR(yieldDef,              "yieldDef");
