// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_STRING(name, "string")` -- a string constant.
// `DEF_SYMBOL(name, "string")` -- an interned symbol and token (empty record).
// `DEF_TOKEN(name, "string")` -- just a token; symbol assumed to exist.
//

DEF_STRING(CH_DOLLAR, "$");
DEF_STRING(CH_DOT,    ".");
DEF_STRING(CH_SLASH,  "/");

DEF_SYMBOL(CH_AT,                 "@");
DEF_SYMBOL(CH_CCURLY,             "}");
DEF_SYMBOL(CH_COLON,              ":");
DEF_SYMBOL(CH_COLONCOLON,         "::");
DEF_SYMBOL(CH_COLONEQUAL,         ":=");
DEF_SYMBOL(CH_COMMA,              ",");
DEF_SYMBOL(CH_CPAREN,             ")");
DEF_SYMBOL(CH_CSQUARE,            "]");
DEF_SYMBOL(CH_DOT,                ".");
DEF_SYMBOL(CH_DOTDOT,             "..");
DEF_SYMBOL(CH_EQUAL,              "=");
DEF_SYMBOL(CH_MINUS,              "-");
DEF_SYMBOL(CH_OCURLY,             "{");
DEF_SYMBOL(CH_OPAREN,             "(");
DEF_SYMBOL(CH_OSQUARE,            "[");
DEF_SYMBOL(CH_PLUS,               "+");
DEF_SYMBOL(CH_QMARK,              "?");
DEF_SYMBOL(CH_RARROW,             "->");
DEF_SYMBOL(CH_SEMICOLON,          ";");
DEF_SYMBOL(CH_SLASH,              "/");
DEF_SYMBOL(CH_STAR,               "*");
DEF_SYMBOL(apply,                 "apply");
DEF_SYMBOL(break,                 "break");
DEF_SYMBOL(classAddMethod,        "classAddMethod");
DEF_SYMBOL(closure,               "closure");
DEF_SYMBOL(continue,              "continue");
DEF_SYMBOL(def,                   "def");
DEF_SYMBOL(directive,             "directive");
DEF_SYMBOL(export,                "export");
DEF_SYMBOL(exportSelection,       "exportSelection");
DEF_SYMBOL(external,              "external");
DEF_SYMBOL(fn,                    "fn");
DEF_SYMBOL(formals,               "formals");
DEF_SYMBOL(format,                "format");
DEF_SYMBOL(function,              "function");
DEF_SYMBOL(identifier,            "identifier");
DEF_SYMBOL(import,                "import");
DEF_SYMBOL(importModule,          "importModule");
DEF_SYMBOL(importModuleSelection, "importModuleSelection");
DEF_SYMBOL(importResource,        "importResource");
DEF_SYMBOL(info,                  "info");
DEF_SYMBOL(int,                   "int");
DEF_SYMBOL(internal,              "internal");
DEF_SYMBOL(interpolate,           "interpolate");
DEF_SYMBOL(keys,                  "keys");
DEF_SYMBOL(language,              "language");
DEF_SYMBOL(literal,               "literal");
DEF_SYMBOL(loadModule,            "loadModule");
DEF_SYMBOL(loadResource,          "loadResource");
DEF_SYMBOL(lvalue,                "lvalue");
DEF_SYMBOL(makeList,              "makeList");
DEF_SYMBOL(makeMap,               "makeMap");
DEF_SYMBOL(makeRecord,            "makeRecord");
DEF_SYMBOL(makeRecordClass,       "makeRecordClass");
DEF_SYMBOL(makeSymbolTable,       "makeSymbolTable");
DEF_SYMBOL(makeValueMap,          "makeValueMap");
DEF_SYMBOL(makeValueSymbolTable,  "makeValueSymbolTable");
DEF_SYMBOL(mapping,               "mapping");
DEF_SYMBOL(maybe,                 "maybe");
DEF_SYMBOL(maybeValue,            "maybeValue");
DEF_SYMBOL(module,                "module");
DEF_SYMBOL(name,                  "name");
DEF_SYMBOL(noYield,               "noYield");
DEF_SYMBOL(nonlocalExit,          "nonlocalExit");
DEF_SYMBOL(null,                  "null");
DEF_SYMBOL(prefix,                "prefix");
DEF_SYMBOL(repeat,                "repeat");
DEF_SYMBOL(return,                "return");
DEF_SYMBOL(select,                "select");
DEF_SYMBOL(source,                "source");
DEF_SYMBOL(statements,            "statements");
DEF_SYMBOL(string,                "string");
DEF_SYMBOL(target,                "target");
DEF_SYMBOL(this,                  "this");
DEF_SYMBOL(top,                   "top");
DEF_SYMBOL(value,                 "value");
DEF_SYMBOL(values,                "values");
DEF_SYMBOL(var,                   "var");
DEF_SYMBOL(varDef,                "varDef");
DEF_SYMBOL(varDefMutable,         "varDefMutable");
DEF_SYMBOL(varRef,                "varRef");
DEF_SYMBOL(void,                  "void");
DEF_SYMBOL(yield,                 "yield");
DEF_SYMBOL(yieldDef,              "yieldDef");
DEF_SYMBOL(zfalse,                "false");  // `z` avoids clash with C.
DEF_SYMBOL(ztrue,                 "true");   // `z` avoids clash with C.

DEF_TOKEN(call,  "call");
DEF_TOKEN(fetch, "fetch");
DEF_TOKEN(store, "store");
