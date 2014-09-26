// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_STRING(name, "string")` -- a string constant.
// `DEF_SYMBOL(name, "string")` -- an interned symbol.
// `DEF_RECORD(name, "string")` -- a record symbol and a token (empty record.
//

DEF_STRING(CH_DOLLAR,            "$");
DEF_STRING(CH_DOT,               ".");
DEF_STRING(CH_SLASH,             "/");

DEF_SYMBOL(CH_PLUS,              "+");
DEF_SYMBOL(CH_QMARK,             "?");
DEF_SYMBOL(CH_STAR,              "*");
DEF_SYMBOL(break,                "break");
DEF_SYMBOL(classAddMethod,       "classAddMethod");
DEF_SYMBOL(continue,             "continue");
DEF_SYMBOL(def,                  "def");
DEF_SYMBOL(export,               "export");
DEF_SYMBOL(fn,                   "fn");
DEF_SYMBOL(formals,              "formals");
DEF_SYMBOL(format,               "format");
DEF_SYMBOL(function,             "function");
DEF_SYMBOL(import,               "import");
DEF_SYMBOL(info,                 "info");
DEF_SYMBOL(interpolate,          "interpolate");
DEF_SYMBOL(keys,                 "keys");
DEF_SYMBOL(language,             "language");
DEF_SYMBOL(loadModule,           "loadModule");
DEF_SYMBOL(loadResource,         "loadResource");
DEF_SYMBOL(lvalue,               "lvalue");
DEF_SYMBOL(makeList,             "makeList");
DEF_SYMBOL(makeMap,              "makeMap");
DEF_SYMBOL(makeRecord,           "makeRecord");
DEF_SYMBOL(makeRecordClass,      "makeRecordClass");
DEF_SYMBOL(makeSymbolTable,      "makeSymbolTable");
DEF_SYMBOL(makeValueMap,         "makeValueMap");
DEF_SYMBOL(makeValueSymbolTable, "makeValueSymbolTable");
DEF_SYMBOL(maybeValue,           "maybeValue");
DEF_SYMBOL(module,               "module");
DEF_SYMBOL(name,                 "name");
DEF_SYMBOL(null,                 "null");
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
DEF_SYMBOL(var,                  "var");
DEF_SYMBOL(void,                 "void");
DEF_SYMBOL(yield,                "yield");
DEF_SYMBOL(yieldDef,             "yieldDef");
DEF_SYMBOL(zfalse,               "false");  // `z` avoids clash with C `false`.
DEF_SYMBOL(ztrue,                "true");   // `z` avoids clash with C `true`.

DEF_RECORD(CH_AT,                 "@");
DEF_RECORD(CH_ATAT,               "@@");
DEF_RECORD(CH_CCURLY,             "}");
DEF_RECORD(CH_COLON,              ":");
DEF_RECORD(CH_COLONCOLON,         "::");
DEF_RECORD(CH_COLONEQUAL,         ":=");
DEF_RECORD(CH_COMMA,              ",");
DEF_RECORD(CH_CPAREN,             ")");
DEF_RECORD(CH_CSQUARE,            "]");
DEF_RECORD(CH_DOT,                ".");
DEF_RECORD(CH_DOTDOT,             "..");
DEF_RECORD(CH_EQUAL,              "=");
DEF_RECORD(CH_MINUS,              "-");
DEF_RECORD(CH_OCURLY,             "{");
DEF_RECORD(CH_OPAREN,             "(");
DEF_RECORD(CH_OSQUARE,            "[");
DEF_RECORD(CH_PLUS,               "+");
DEF_RECORD(CH_QMARK,              "?");
DEF_RECORD(CH_RARROW,             "->");
DEF_RECORD(CH_SEMICOLON,          ";");
DEF_RECORD(CH_SLASH,              "/");
DEF_RECORD(CH_STAR,               "*");
DEF_RECORD(apply,                 "apply");
DEF_RECORD(break,                 "break");
DEF_RECORD(call,                  "call");
DEF_RECORD(closure,               "closure");
DEF_RECORD(continue,              "continue");
DEF_RECORD(def,                   "def");
DEF_RECORD(directive,             "directive");
DEF_RECORD(export,                "export");
DEF_RECORD(exportSelection,       "exportSelection");
DEF_RECORD(external,              "external");
DEF_RECORD(fetch,                 "fetch");
DEF_RECORD(fn,                    "fn");
DEF_RECORD(identifier,            "identifier");
DEF_RECORD(import,                "import");
DEF_RECORD(importModule,          "importModule");
DEF_RECORD(importModuleSelection, "importModuleSelection");
DEF_RECORD(importResource,        "importResource");
DEF_RECORD(int,                   "int");
DEF_RECORD(internal,              "internal");
DEF_RECORD(literal,               "literal");
DEF_RECORD(mapping,               "mapping");
DEF_RECORD(maybe,                 "maybe");
DEF_RECORD(module,                "module");
DEF_RECORD(noYield,               "noYield");
DEF_RECORD(nonlocalExit,          "nonlocalExit");
DEF_RECORD(null,                  "null");
DEF_RECORD(return,                "return");
DEF_RECORD(store,                 "store");
DEF_RECORD(string,                "string");
DEF_RECORD(var,                   "var");
DEF_RECORD(varDef,                "varDef");
DEF_RECORD(varDefMutable,         "varDefMutable");
DEF_RECORD(varRef,                "varRef");
DEF_RECORD(void,                  "void");
DEF_RECORD(yield,                 "yield");
DEF_RECORD(zfalse,                "false");  // `z` avoids clash with C `false`.
DEF_RECORD(ztrue,                 "true");   // `z` avoids clash with C `true`.
