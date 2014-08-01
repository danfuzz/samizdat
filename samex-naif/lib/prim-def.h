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
PRIM_DEF(String,                  CLS_String);
PRIM_DEF(Uniqlet,                 CLS_Uniqlet);
PRIM_DEF(Value,                   CLS_Value);

// Generic functions for all values.
PRIM_DEF(debugName,               SEL_NAME(debugName));
PRIM_DEF(debugString,             SEL_NAME(debugString));
PRIM_DEF(perEq,                   SEL_NAME(perEq));
PRIM_DEF(perOrder,                SEL_NAME(perOrder));
PRIM_DEF(totalEq,                 SEL_NAME(totalEq));
PRIM_DEF(totalOrder,              SEL_NAME(totalOrder));

// Generic functions: One-Offs, exported to the global environment.
PRIM_DEF(cat,                     SEL_NAME(cat));
PRIM_DEF(get,                     SEL_NAME(get));
PRIM_DEF(get_key,                 SEL_NAME(get_key));
PRIM_DEF(get_size,                SEL_NAME(get_size));
PRIM_DEF(get_value,               SEL_NAME(get_value));
PRIM_DEF(nth,                     SEL_NAME(nth));
PRIM_DEF(toInt,                   SEL_NAME(toInt));
PRIM_DEF(toNumber,                SEL_NAME(toNumber));
PRIM_DEF(toString,                SEL_NAME(toString));

// Generic functions: `Bitwise` protocol, intended for modularization.
PRIM_DEF(Bitwise_and,             SEL_NAME(and));
PRIM_DEF(Bitwise_bit,             SEL_NAME(bit));
PRIM_DEF(Bitwise_bitSize,         SEL_NAME(bitSize));
PRIM_DEF(Bitwise_not,             SEL_NAME(not));
PRIM_DEF(Bitwise_or,              SEL_NAME(or));
PRIM_DEF(Bitwise_shl,             SEL_NAME(shl));
PRIM_DEF(Bitwise_xor,             SEL_NAME(xor));

// Generic functions: `Box` protocol.
PRIM_DEF(store,                   SEL_NAME(store));

// Generic functions: `Object` class.
PRIM_DEF(objectDataOf,            SEL_NAME(objectDataOf));

// Generic functions: `DerivedData` class.
PRIM_DEF(dataOf,                  SEL_NAME(dataOf));

// Generic functions: `Function` protocol.
PRIM_DEF(call,                    SEL_NAME(call));

// Generic functions: `Collection` protocol, intended for modularization.
PRIM_DEF(Collection_del,          SEL_NAME(del));
PRIM_DEF(Collection_keyList,      SEL_NAME(keyList));
PRIM_DEF(Collection_nthMapping,   SEL_NAME(nthMapping));
PRIM_DEF(Collection_put,          SEL_NAME(put));
PRIM_DEF(Collection_valueList,    SEL_NAME(valueList));

// Generic functions: `Sequence` protocol, intended for modularization.
PRIM_DEF(Sequence_reverse,        SEL_NAME(reverse));
PRIM_DEF(Sequence_sliceExclusive, SEL_NAME(sliceExclusive));
PRIM_DEF(Sequence_sliceInclusive, SEL_NAME(sliceInclusive));

// Generic functions: `Generator` protocol, intended for modularization but
// also exported to the global environment.
PRIM_DEF(collect,                 SEL_NAME(collect));
PRIM_DEF(fetch,                   SEL_NAME(fetch));
PRIM_DEF(nextValue,               SEL_NAME(nextValue));

// Generic functions: `Number` protocol, intended for modularization.
PRIM_DEF(Number_abs,              SEL_NAME(abs));
PRIM_DEF(Number_add,              SEL_NAME(add));
PRIM_DEF(Number_div,              SEL_NAME(div));
PRIM_DEF(Number_divEu,            SEL_NAME(divEu));
PRIM_DEF(Number_mod,              SEL_NAME(mod));
PRIM_DEF(Number_modEu,            SEL_NAME(modEu));
PRIM_DEF(Number_mul,              SEL_NAME(mul));
PRIM_DEF(Number_neg,              SEL_NAME(neg));
PRIM_DEF(Number_sign,             SEL_NAME(sign));
PRIM_DEF(Number_sub,              SEL_NAME(sub));

// Primitive functions: directly exported.
PRIM_DEF(makeData,                FUN_DerivedData_makeData);
PRIM_DEF(makeObject,              FUN_Object_makeObject);
PRIM_DEF(makeObjectClass,         FUN_Object_makeObjectClass);
PRIM_FUNC(className,              1, 1);
PRIM_FUNC(classParent,            1, 1);
PRIM_FUNC(die,                    0, -1);
PRIM_FUNC(eq,                     2, 2);
PRIM_FUNC(genericBind,            3, 3);
PRIM_FUNC(get_class,              1, 1);
PRIM_FUNC(hasClass,               2, 2);
PRIM_FUNC(ifIs,                   2, 3);
PRIM_FUNC(ifNot,                  2, 2);
PRIM_FUNC(ifSwitch,               2, 4);
PRIM_FUNC(ifValue,                2, 3);
PRIM_FUNC(ifValueOr,              1, -1);
PRIM_FUNC(ifValues,               2, 3);
PRIM_FUNC(loop,                   1, 1);
PRIM_FUNC(makeDerivedDataClass,   1, 1);
PRIM_FUNC(makeGeneric,            2, 3);
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
