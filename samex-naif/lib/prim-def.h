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

// Types.
PRIM_DEF(Data,                    TYPE_Data);
PRIM_DEF(DerivedData,             TYPE_DerivedData);
PRIM_DEF(Int,                     TYPE_Int);
PRIM_DEF(List,                    TYPE_List);
PRIM_DEF(Map,                     TYPE_Map);
PRIM_DEF(String,                  TYPE_String);
PRIM_DEF(Type,                    TYPE_Type);
PRIM_DEF(Uniqlet,                 TYPE_Uniqlet);
PRIM_DEF(Value,                   TYPE_Value);

// Generic functions for all values.
PRIM_DEF(debugName,               GFN_debugName);
PRIM_DEF(debugString,             GFN_debugString);
PRIM_DEF(perEq,                   GFN_perEq);
PRIM_DEF(perOrder,                GFN_perOrder);
PRIM_DEF(totalEq,                 GFN_totalEq);
PRIM_DEF(totalOrder,              GFN_totalOrder);

// Generic functions: One-Offs, exported to the global environment.
PRIM_DEF(cat,                     GFN_cat);
PRIM_DEF(get,                     GFN_get);
PRIM_DEF(get_key,                 GFN_get_key);
PRIM_DEF(get_size,                GFN_get_size);
PRIM_DEF(get_value,               GFN_get_value);
PRIM_DEF(nth,                     GFN_nth);
PRIM_DEF(toInt,                   GFN_toInt);
PRIM_DEF(toNumber,                GFN_toNumber);
PRIM_DEF(toString,                GFN_toString);

// Generic functions: `Bitwise` protocol, intended for modularization.
PRIM_DEF(Bitwise_and,             GFN_and);
PRIM_DEF(Bitwise_bit,             GFN_bit);
PRIM_DEF(Bitwise_bitSize,         GFN_bitSize);
PRIM_DEF(Bitwise_not,             GFN_not);
PRIM_DEF(Bitwise_or,              GFN_or);
PRIM_DEF(Bitwise_shl,             GFN_shl);
PRIM_DEF(Bitwise_xor,             GFN_xor);

// Generic functions: `Box` protocol.
PRIM_DEF(fetch,                   GFN_fetch);
PRIM_DEF(store,                   GFN_store);

// Generic functions: `Function` protocol.
PRIM_DEF(call,                    GFN_call);

// Generic functions: `Collection` protocol, intended for modularization.
PRIM_DEF(Collection_del,          GFN_del);
PRIM_DEF(Collection_keyList,      GFN_keyList);
PRIM_DEF(Collection_nthMapping,   GFN_nthMapping);
PRIM_DEF(Collection_put,          GFN_put);
PRIM_DEF(Collection_valueList,    GFN_valueList);

// Generic functions: `Sequence` protocol, intended for modularization.
PRIM_DEF(Sequence_reverse,        GFN_reverse);
PRIM_DEF(Sequence_sliceExclusive, GFN_sliceExclusive);
PRIM_DEF(Sequence_sliceInclusive, GFN_sliceInclusive);

// Generic functions: `Generator` protocol, intended for modularization but
// also exported to the global environment.
PRIM_DEF(collect,                 GFN_collect);
PRIM_DEF(nextValue,               GFN_nextValue);

// Generic functions: `Number` protocol, intended for modularization.
PRIM_DEF(Number_abs,              GFN_abs);
PRIM_DEF(Number_add,              GFN_add);
PRIM_DEF(Number_div,              GFN_div);
PRIM_DEF(Number_divEu,            GFN_divEu);
PRIM_DEF(Number_mod,              GFN_mod);
PRIM_DEF(Number_modEu,            GFN_modEu);
PRIM_DEF(Number_mul,              GFN_mul);
PRIM_DEF(Number_neg,              GFN_neg);
PRIM_DEF(Number_sign,             GFN_sign);
PRIM_DEF(Number_sub,              GFN_sub);

// Primitive functions: directly exported.
PRIM_FUNC(dataOf,                 1, 2);
PRIM_FUNC(die,                    0, -1);
PRIM_FUNC(eq,                     2, 2);
PRIM_FUNC(genericBind,            3, 3);
PRIM_FUNC(get_type,               1, 1);
PRIM_FUNC(hasType,                2, 2);
PRIM_FUNC(ifIs,                   2, 3);
PRIM_FUNC(ifNot,                  2, 2);
PRIM_FUNC(ifSwitch,               2, 4);
PRIM_FUNC(ifValue,                2, 3);
PRIM_FUNC(ifValueOr,              1, -1);
PRIM_FUNC(ifValues,               2, 3);
PRIM_FUNC(interpolate,            1, 1);
PRIM_FUNC(loop,                   1, 1);
PRIM_FUNC(makeDerivedDataType,    1, 1);
PRIM_FUNC(makeList,               0, -1);
PRIM_FUNC(makeRegularGeneric,     2, 3);
PRIM_FUNC(makeUniqlet,            0, 0);
PRIM_FUNC(makeUnitypeGeneric,     2, 3);
PRIM_FUNC(makeValue,              1, 2);
PRIM_FUNC(makeValueMap,           1, -1);
PRIM_FUNC(maybeValue,             1, 1);
PRIM_FUNC(note,                   0, -1);
PRIM_FUNC(order,                  2, 2);
PRIM_FUNC(typeName,               1, 1);
PRIM_FUNC(typeParent,             1, 1);

// Primitive functions: intended for modularization
PRIM_DEF(Generator_stdCollect,    FUN_Generator_stdCollect);
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
