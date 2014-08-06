// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_STRING(name, "string")` defines a string constant.
// `DEF_DATA(name, "string")` defines a string and a derived data class.
// `DEF_TOKEN(name, "string")` defines a string, a class, and a token constant.
//
// Token constants are class-only transparent derived values, whose classes
// are named with the indicated strings.
//

DEF_TOKEN(CH_AT,                 "@");
DEF_TOKEN(CH_ATAT,               "@@");
DEF_TOKEN(CH_CCURLY,             "}");
DEF_TOKEN(CH_CPAREN,             ")");
DEF_TOKEN(CH_CSQUARE,            "]");
DEF_TOKEN(CH_COLON,              ":");
DEF_TOKEN(CH_COLONCOLON,         "::");
DEF_TOKEN(CH_COLONEQUAL,         ":=");
DEF_TOKEN(CH_COMMA,              ",");
DEF_TOKEN(CH_DOLLAR,             "$");
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

DEF_DATA(MapGenerator,           "MapGenerator");
DEF_DATA(SequenceGenerator,      "SequenceGenerator");
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

DEF_STRING(absent,               "absent");
DEF_STRING(bind,                 "bind");
DEF_STRING(cat,                  "cat");
DEF_STRING(collect,              "collect");
DEF_STRING(directory,            "directory");
DEF_STRING(exports,              "exports");
DEF_STRING(file,                 "file");
DEF_STRING(formals,              "formals");
DEF_STRING(format,               "format");
DEF_STRING(function,             "function");
DEF_STRING(genericBind,          "genericBind");
DEF_STRING(get,                  "get");
DEF_STRING(imports,              "imports");
DEF_STRING(index,                "index");
DEF_STRING(info,                 "info");
DEF_STRING(interpolate,          "interpolate");
DEF_STRING(language,             "language");
DEF_STRING(loadModule,           "loadModule");
DEF_STRING(loadResource,         "loadResource");
DEF_STRING(lvalue,               "lvalue");
DEF_STRING(main,                 "main");
DEF_STRING(makeData,             "makeData");
DEF_STRING(makeDerivedDataClass, "makeDerivedDataClass");
DEF_STRING(makeGeneric,          "makeGeneric");
DEF_STRING(makeList,             "makeList");
DEF_STRING(makeValueMap,         "makeValueMap");
DEF_STRING(map,                  "map");
DEF_STRING(maybeValue,           "maybeValue");
DEF_STRING(name,                 "name");
DEF_STRING(other,                "other");
DEF_STRING(prefix,               "prefix");
DEF_STRING(repeat,               "repeat");
DEF_STRING(resources,            "resources");
DEF_STRING(runCommandLine,       "runCommandLine");
DEF_STRING(select,               "select");
DEF_STRING(seq,                  "seq");
DEF_STRING(source,               "source");
DEF_STRING(statements,           "statements");
DEF_STRING(symlink,              "symlink");
DEF_STRING(target,               "target");
DEF_STRING(this,                 "this");
DEF_STRING(top,                  "top");
DEF_STRING(value,                "value");
DEF_STRING(values,               "values");
DEF_STRING(yieldDef,             "yieldDef");
