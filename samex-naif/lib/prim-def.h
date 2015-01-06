// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `PRIM_DEF(name, value)` binds a name to a pre-existing value.
//
// `PRIM_FUNC(name, minArgs, maxArgs)` defines a primitive function with the
// given name and argument restrictions.
//
// **Note:** This file gets `#include`d multiple times, and so does not
// have the usual guard macros.
//

// Classes.
PRIM_DEF(Bool,                    CLS_Bool);
PRIM_DEF(Box,                     CLS_Box);
PRIM_DEF(Cell,                    CLS_Cell);
PRIM_DEF(Class,                   CLS_Class);
PRIM_DEF(Cmp,                     CLS_Cmp);
PRIM_DEF(Core,                    CLS_Core);
PRIM_DEF(If,                      CLS_If);
PRIM_DEF(Int,                     CLS_Int);
PRIM_DEF(Lazy,                    CLS_Lazy);
PRIM_DEF(List,                    CLS_List);
PRIM_DEF(Map,                     CLS_Map);
PRIM_DEF(Metaclass,               CLS_Metaclass);
PRIM_DEF(Null,                    CLS_Null);
PRIM_DEF(NullBox,                 CLS_NullBox);
PRIM_DEF(Object,                  CLS_Object);
PRIM_DEF(Promise,                 CLS_Promise);
PRIM_DEF(Record,                  CLS_Record);
PRIM_DEF(Result,                  CLS_Result);
PRIM_DEF(String,                  CLS_String);
PRIM_DEF(Symbol,                  CLS_Symbol);
PRIM_DEF(SymbolTable,             CLS_SymbolTable);
PRIM_DEF(Value,                   CLS_Value);

// Constants
PRIM_DEF(nullBox,                 THE_NULL_BOX);
PRIM_DEF(voidResult,              THE_VOID_RESULT);

// Primitive functions: directly exported.
PRIM_FUNC(die,                    0, -1);
PRIM_FUNC(note,                   0, -1);

// Primitive functions: intended for modularization
PRIM_DEF(Generator_stdCollect,    FUN_Generator_stdCollect);
PRIM_DEF(Generator_stdFetch,      FUN_Generator_stdFetch);
PRIM_DEF(Generator_stdForEach,    FUN_Generator_stdForEach);
PRIM_FUNC(Code_eval,              2, 2);
PRIM_FUNC(Code_evalBinary,        2, 2);
PRIM_FUNC(Io0_cwd,                0, 0);
PRIM_FUNC(Io0_fileType,           1, 1);
PRIM_FUNC(Io0_readDirectory,      1, 1);
PRIM_FUNC(Io0_readFileUtf8,       1, 1);
PRIM_FUNC(Io0_readLink,           1, 1);
PRIM_FUNC(Io0_writeFileUtf8,      2, 2);
PRIM_FUNC(Lang0_languageOf,       1, 1);
PRIM_FUNC(Lang0_parseExpression,  1, 1);
PRIM_FUNC(Lang0_parseProgram,     1, 1);
PRIM_FUNC(Lang0_simplify,         2, 2);
PRIM_FUNC(Lang0_tokenize,         1, 1);
