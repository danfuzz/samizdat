// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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
PRIM_DEF(Class,                   CLS_Class);
PRIM_DEF(Data,                    CLS_Data);
PRIM_DEF(DerivedData,             CLS_DerivedData);
PRIM_DEF(Int,                     CLS_Int);
PRIM_DEF(List,                    CLS_List);
PRIM_DEF(Map,                     CLS_Map);
PRIM_DEF(Null,                    CLS_Null);
PRIM_DEF(Selector,                CLS_Selector);
PRIM_DEF(String,                  CLS_String);
PRIM_DEF(Uniqlet,                 CLS_Uniqlet);
PRIM_DEF(Value,                   CLS_Value);

// Methods for all values.
PRIM_DEF(perEq,                   SEL_NAME(perEq));
PRIM_DEF(perOrder,                SEL_NAME(perOrder));
PRIM_DEF(totalEq,                 SEL_NAME(totalEq));
PRIM_DEF(totalOrder,              SEL_NAME(totalOrder));

// Methods: One-Offs, exported to the global environment.
PRIM_DEF(cat,                     SEL_NAME(cat));
PRIM_DEF(get,                     SEL_NAME(get));

// Methods: `Object` class.
PRIM_DEF(objectDataOf,            SEL_NAME(objectDataOf));

// Methods: `DerivedData` class.
PRIM_DEF(dataOf,                  SEL_NAME(dataOf));

// Primitive functions: directly exported.
PRIM_DEF(makeAnonymousSelector,   FUN_Selector_makeAnonymousSelector);
PRIM_DEF(makeData,                FUN_DerivedData_makeData);
PRIM_DEF(makeDerivedDataClass,    FUN_DerivedData_makeDerivedDataClass);
PRIM_DEF(makeObject,              FUN_Object_makeObject);
PRIM_DEF(makeObjectClass,         FUN_Object_makeObjectClass);
PRIM_DEF(selectorFromName,        FUN_Selector_selectorFromName);
PRIM_DEF(selectorIsInterned,      FUN_Selector_selectorIsInterned);
PRIM_DEF(selectorName,            FUN_Selector_selectorName);
PRIM_FUNC(classAddMethod,         3, 3);
PRIM_FUNC(className,              1, 1);
PRIM_FUNC(classParent,            1, 1);
PRIM_FUNC(die,                    0, -1);
PRIM_FUNC(eq,                     2, 2);
PRIM_FUNC(get_class,              1, 1);
PRIM_FUNC(hasClass,               2, 2);
PRIM_FUNC(ifIs,                   2, 3);
PRIM_FUNC(ifNot,                  2, 2);
PRIM_FUNC(ifSwitch,               2, 4);
PRIM_FUNC(ifValue,                2, 3);
PRIM_FUNC(ifValueOr,              1, -1);
PRIM_FUNC(ifValues,               2, 3);
PRIM_FUNC(loop,                   1, 1);
PRIM_FUNC(makeList,               0, -1);
PRIM_FUNC(makeUniqlet,            0, 0);
PRIM_FUNC(makeValueMap,           1, -1);
PRIM_FUNC(maybeValue,             1, 1);
PRIM_FUNC(note,                   0, -1);
PRIM_FUNC(order,                  2, 2);

// Primitive functions: intended for modularization
PRIM_DEF(Generator_stdCollect,    FUN_Generator_stdCollect);
PRIM_DEF(Generator_stdFetch,      FUN_Generator_stdFetch);
PRIM_FUNC(Box_makeCell,           0, 1);
PRIM_FUNC(Box_makePromise,        0, 0);
PRIM_FUNC(Box_makeResult,         0, 1);
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
