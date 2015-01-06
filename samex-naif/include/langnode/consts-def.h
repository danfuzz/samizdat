// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//
// `DEF_LITERAL(name, value)` -- an arbitrary literal value.
// `DEF_STRING(name, "string")` -- a string constant.
// `DEF_SYMBOL(name, "string")` -- an interned symbol and token (empty record).
// `DEF_TOKEN(name)` -- just a token; symbol assumed to exist.
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
DEF_SYMBOL(box,                   "box");
DEF_SYMBOL(break,                 "break");
DEF_SYMBOL(cell,                  "cell");
DEF_SYMBOL(class,                 "class");
DEF_SYMBOL(classMethod,           "classMethod");
DEF_SYMBOL(closure,               "closure");
DEF_SYMBOL(continue,              "continue");
DEF_SYMBOL(def,                   "def");
DEF_SYMBOL(directive,             "directive");
DEF_SYMBOL(export,                "export");
DEF_SYMBOL(exportSelection,       "exportSelection");
DEF_SYMBOL(external,              "external");
DEF_SYMBOL(fn,                    "fn");
DEF_SYMBOL(formal,                "formal");
DEF_SYMBOL(formals,               "formals");
DEF_SYMBOL(format,                "format");
DEF_SYMBOL(function,              "function");
DEF_SYMBOL(identifier,            "identifier");
DEF_SYMBOL(import,                "import");
DEF_SYMBOL(importModule,          "importModule");
DEF_SYMBOL(importModuleSelection, "importModuleSelection");
DEF_SYMBOL(importResource,        "importResource");
DEF_SYMBOL(info,                  "info");
DEF_SYMBOL(instanceMethod,        "instanceMethod");
DEF_SYMBOL(int,                   "int");
DEF_SYMBOL(internal,              "internal");
DEF_SYMBOL(interpolate,           "interpolate");
DEF_SYMBOL(keys,                  "keys");
DEF_SYMBOL(language,              "language");
DEF_SYMBOL(lazy,                  "lazy");
DEF_SYMBOL(literal,               "literal");
DEF_SYMBOL(loadModule,            "loadModule");
DEF_SYMBOL(loadResource,          "loadResource");
DEF_SYMBOL(lvalue,                "lvalue");
DEF_SYMBOL(mapping,               "mapping");
DEF_SYMBOL(maybe,                 "maybe");
DEF_SYMBOL(methodId,              "methodId");
DEF_SYMBOL(module,                "module");
DEF_SYMBOL(name,                  "name");
DEF_SYMBOL(noYield,               "noYield");
DEF_SYMBOL(nonlocalExit,          "nonlocalExit");
DEF_SYMBOL(null,                  "null");
DEF_SYMBOL(prefix,                "prefix");
DEF_SYMBOL(promise,               "promise");
DEF_SYMBOL(result,                "result");
DEF_SYMBOL(return,                "return");
DEF_SYMBOL(select,                "select");
DEF_SYMBOL(source,                "source");
DEF_SYMBOL(statements,            "statements");
DEF_SYMBOL(string,                "string");
DEF_SYMBOL(target,                "target");
DEF_SYMBOL(this,                  "this");
DEF_SYMBOL(top,                   "top");
DEF_SYMBOL(values,                "values");
DEF_SYMBOL(var,                   "var");
DEF_SYMBOL(varDef,                "varDef");
DEF_SYMBOL(varRef,                "varRef");
DEF_SYMBOL(void,                  "void");
DEF_SYMBOL(yield,                 "yield");
DEF_SYMBOL(yieldDef,              "yieldDef");
DEF_SYMBOL(zfalse,                "false");  // `z` avoids clash with C.
DEF_SYMBOL(ztrue,                 "true");   // `z` avoids clash with C.

DEF_TOKEN(call);
DEF_TOKEN(fetch);
DEF_TOKEN(maybeValue);
DEF_TOKEN(repeat);
DEF_TOKEN(store);
DEF_TOKEN(value);

// Literals have to be defined after everything else, in particular after the
// constants used during calls to `makeLiteral()`.
DEF_LITERAL(EMPTY_LIST,         EMPTY_LIST);
DEF_LITERAL(EMPTY_MAP,          EMPTY_MAP);
DEF_LITERAL(EMPTY_SYMBOL_TABLE, EMPTY_SYMBOL_TABLE);
DEF_LITERAL(If,                 CLS_If);
DEF_LITERAL(List,               CLS_List);
DEF_LITERAL(Map,                CLS_Map);
DEF_LITERAL(Object,             CLS_Object);
DEF_LITERAL(Record,             CLS_Record);
DEF_LITERAL(SymbolTable,        CLS_SymbolTable);
DEF_LITERAL(null,               THE_NULL);
DEF_LITERAL(zfalse,             BOOL_FALSE);  // `z` avoids clash with C.
DEF_LITERAL(ztrue,              BOOL_TRUE);   // `z` avoids clash with C.
