// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_STRING(name, "string")` -- a string constant.
// `DEF_SYMBOL(name, "string")` -- an interned symbol.
// `DEF_DATA(name, "string")` -- a derived data class.
// `DEF_TOKEN(name, "string")` -- a derived data class and a token.
//
// Tokens are class-only transparent derived values, whose classes
// are named with the indicated strings.
//

DEF_STRING(CH_DOLLAR,            "$");
DEF_STRING(CH_DOT,               ".");
DEF_STRING(CH_PLUS,              "+");
DEF_STRING(CH_QMARK,             "?");
DEF_STRING(CH_SLASH,             "/");
DEF_STRING(CH_STAR,              "*");
DEF_STRING(break,                "break");
DEF_STRING(continue,             "continue");
DEF_STRING(def,                  "def");
DEF_STRING(export,               "export");
DEF_STRING(fn,                   "fn");
DEF_STRING(import,               "import");
DEF_STRING(language,             "language");
DEF_STRING(null,                 "null");
DEF_STRING(return,               "return");
DEF_STRING(var,                  "var");
DEF_STRING(void,                 "void");
DEF_STRING(yield,                "yield");
DEF_STRING(zfalse,               "false");  // `z` avoids clash with C `false`.
DEF_STRING(ztrue,                "true");   // `z` avoids clash with C `true`.

DEF_SYMBOL(classAddMethod,       "classAddMethod");
DEF_SYMBOL(formals,              "formals");
DEF_SYMBOL(format,               "format");
DEF_SYMBOL(function,             "function");
DEF_SYMBOL(info,                 "info");
DEF_SYMBOL(interpolate,          "interpolate");
DEF_SYMBOL(loadModule,           "loadModule");
DEF_SYMBOL(loadResource,         "loadResource");
DEF_SYMBOL(lvalue,               "lvalue");
DEF_SYMBOL(makeData,             "makeData");
DEF_SYMBOL(makeDerivedDataClass, "makeDerivedDataClass");
DEF_SYMBOL(makeList,             "makeList");
DEF_SYMBOL(makeValueMap,         "makeValueMap");
DEF_SYMBOL(maybeValue,           "maybeValue");
DEF_SYMBOL(name,                 "name");
DEF_SYMBOL(prefix,               "prefix");
DEF_SYMBOL(repeat,               "repeat");
DEF_SYMBOL(return,               "return");
DEF_SYMBOL(select,               "select");
DEF_SYMBOL(source,               "source");
DEF_SYMBOL(statements,           "statements");
DEF_SYMBOL(target,               "target");
DEF_SYMBOL(this,                 "this");
DEF_SYMBOL(top,                  "top");
DEF_SYMBOL(value,                "value");
DEF_SYMBOL(values,               "values");
DEF_SYMBOL(yield,                "yield");
DEF_SYMBOL(yieldDef,             "yieldDef");

DEF_TOKEN(CH_AT,                 "@");
DEF_TOKEN(CH_ATAT,               "@@");
DEF_TOKEN(CH_CCURLY,             "}");
DEF_TOKEN(CH_CPAREN,             ")");
DEF_TOKEN(CH_CSQUARE,            "]");
DEF_TOKEN(CH_COLON,              ":");
DEF_TOKEN(CH_COLONCOLON,         "::");
DEF_TOKEN(CH_COLONEQUAL,         ":=");
DEF_TOKEN(CH_COMMA,              ",");
DEF_TOKEN(CH_DOT,                ".");
DEF_TOKEN(CH_DOTDOT,             "..");
DEF_TOKEN(CH_EQUAL,              "=");
DEF_TOKEN(CH_MINUS,              "-");
DEF_TOKEN(CH_OCURLY,             "{");
DEF_TOKEN(CH_OPAREN,             "(");
DEF_TOKEN(CH_OSQUARE,            "[");
DEF_TOKEN(CH_PLUS,               "+");
DEF_TOKEN(CH_QMARK,              "?");
DEF_TOKEN(CH_RARROW,             "->");
DEF_TOKEN(CH_SEMICOLON,          ";");
DEF_TOKEN(CH_SLASH,              "/");
DEF_TOKEN(CH_STAR,               "*");
DEF_TOKEN(break,                 "break");
DEF_TOKEN(continue,              "continue");
DEF_TOKEN(def,                   "def");
DEF_TOKEN(export,                "export");
DEF_TOKEN(fn,                    "fn");
DEF_TOKEN(import,                "import");
DEF_TOKEN(null,                  "null");
DEF_TOKEN(return,                "return");
DEF_TOKEN(var,                   "var");
DEF_TOKEN(void,                  "void");
DEF_TOKEN(yield,                 "yield");
DEF_TOKEN(zfalse,                "false");  // `z` avoids clash with C `false`.
DEF_TOKEN(ztrue,                 "true");   // `z` avoids clash with C `true`.

DEF_DATA(apply,                  "apply");
DEF_DATA(call,                   "call");
DEF_DATA(closure,                "closure");
DEF_DATA(directive,              "directive");
DEF_DATA(exportSelection,        "exportSelection");
DEF_DATA(external,               "external");
DEF_DATA(fetch,                  "fetch");
DEF_DATA(identifier,             "identifier");
DEF_DATA(importModule,           "importModule");
DEF_DATA(importModuleSelection,  "importModuleSelection");
DEF_DATA(importResource,         "importResource");
DEF_DATA(int,                    "int");
DEF_DATA(internal,               "internal");
DEF_DATA(literal,                "literal");
DEF_DATA(maybe,                  "maybe");
DEF_DATA(module,                 "module");
DEF_DATA(noYield,                "noYield");
DEF_DATA(nonlocalExit,           "nonlocalExit");
DEF_DATA(store,                  "store");
DEF_DATA(string,                 "string");
DEF_DATA(varDef,                 "varDef");
DEF_DATA(varDefMutable,          "varDefMutable");
DEF_DATA(varRef,                 "varRef");
