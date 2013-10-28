/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `PRIM_DEF(name, value)` binds a name to a pre-existing value.
 *
 * `PRIM_FUNC(name, minArgs, maxArgs)` defines a primitive function with the
 * given name and argument restrictions.
 *
 * **Note:** This file gets `#include`d multiple times, and so does not
 * have the usual guard macros.
 */

// Types
PRIM_DEF(Int,        TYPE_Int);
PRIM_DEF(List,       TYPE_List);
PRIM_DEF(Map,        TYPE_Map);
PRIM_DEF(String,     TYPE_String);
PRIM_DEF(Type,       TYPE_Type);
PRIM_DEF(Uniqlet,    TYPE_Uniqlet);
PRIM_DEF(Value,      TYPE_Value);

// Generic functions for all values
PRIM_DEF(perEq,      GFN_perEq);
PRIM_DEF(perOrder,   GFN_perOrder);
PRIM_DEF(totEq,      GFN_totEq);
PRIM_DEF(totOrder,   GFN_totOrder);

// Generic functions: One-Offs
PRIM_DEF(keyOf,      GFN_keyOf);
PRIM_DEF(nameOf,     GFN_nameOf);
PRIM_DEF(toInt,      GFN_toInt);
PRIM_DEF(toNumber,   GFN_toNumber);
PRIM_DEF(toString,   GFN_toString);
PRIM_DEF(valueOf,    GFN_valueOf);

// Generic functions: `Bitwise` protocol
PRIM_DEF(Bitwise_and,     GFN_and);
PRIM_DEF(Bitwise_bit,     GFN_bit);
PRIM_DEF(Bitwise_bitSize, GFN_bitSize);
PRIM_DEF(Bitwise_not,     GFN_not);
PRIM_DEF(Bitwise_or,      GFN_or);
PRIM_DEF(Bitwise_shl,     GFN_shl);
PRIM_DEF(Bitwise_xor,     GFN_xor);

// Generic functions: `Box` protocol, intended for modularization
PRIM_DEF(Box_canStore, GFN_canStore);
PRIM_DEF(Box_fetch,    GFN_fetch);
PRIM_DEF(Box_store,    GFN_store);

// Generic functions: `Function` protocol
PRIM_DEF(call,       GFN_call);
PRIM_DEF(canCall,    GFN_canCall);

// Generic functions: `Collection` protocol
PRIM_DEF(cat,        GFN_cat);
PRIM_DEF(del,        GFN_del);
PRIM_DEF(get,        GFN_get);
PRIM_DEF(keyList,    GFN_keyList);
PRIM_DEF(nth,        GFN_nth);
PRIM_DEF(put,        GFN_put);
PRIM_DEF(sizeOf,     GFN_sizeOf);
PRIM_DEF(slice,      GFN_slice);

// Generic functions: `Sequence` protocol, intended for modularization
PRIM_DEF(Sequence_reverse,    GFN_reverse);

// Generic functions: `Generator` protocol, intended for modularization
PRIM_DEF(Generator_collect,   GFN_collect);
PRIM_DEF(Generator_filter,    GFN_filter);
PRIM_DEF(Generator_nextValue, GFN_nextValue);

// Generic functions: `Number` protocol, intended for modularization
PRIM_DEF(Number_abs,          GFN_abs);
PRIM_DEF(Number_add,          GFN_add);
PRIM_DEF(Number_div,          GFN_div);
PRIM_DEF(Number_divEu,        GFN_divEu);
PRIM_DEF(Number_mod,          GFN_mod);
PRIM_DEF(Number_modEu,        GFN_modEu);
PRIM_DEF(Number_mul,          GFN_mul);
PRIM_DEF(Number_neg,          GFN_neg);
PRIM_DEF(Number_sign,         GFN_sign);
PRIM_DEF(Number_sub,          GFN_sub);

// Primitive functions: directly exported
PRIM_FUNC(dataOf,             1, 2);
PRIM_FUNC(eq,                 2, 2);
PRIM_FUNC(ge,                 2, 2);
PRIM_FUNC(genericBind,        3, 3);
PRIM_FUNC(gt,                 2, 2);
PRIM_FUNC(hasType,            2, 2);
PRIM_FUNC(ifIs,               2, 3);
PRIM_FUNC(ifNot,              2, 2);
PRIM_FUNC(ifValue,            2, 3);
PRIM_FUNC(ifValueOr,          2, 2);
PRIM_FUNC(ifValues,           2, 3);
PRIM_FUNC(le,                 2, 2);
PRIM_FUNC(loop,               1, 1);
PRIM_FUNC(loopReduce,         1, -1);
PRIM_FUNC(lt,                 2, 2);
PRIM_FUNC(makeList,           0, -1);
PRIM_FUNC(makeRegularGeneric, 2, 3);
PRIM_FUNC(makeUniqlet,        0, 0);
PRIM_FUNC(makeUnitypeGeneric, 2, 3);
PRIM_FUNC(makeValue,          1, 2);
PRIM_FUNC(makeValueMap,       1, -1);
PRIM_FUNC(ne,                 2, 2);
PRIM_FUNC(nonlocalExit,       1, 2);
PRIM_FUNC(optValue,           1, 1);
PRIM_FUNC(samEval,            2, 2);
PRIM_FUNC(samParseExpression, 1, 1);
PRIM_FUNC(samParseProgram,    1, 1);
PRIM_FUNC(samTokenize,        1, 1);
PRIM_FUNC(totalOrder,         2, 2);
PRIM_FUNC(typeOf,             1, 1);

// Primitive functions: intended for modularization
PRIM_FUNC(Box_makeMutableBox,    0, 1);
PRIM_FUNC(Box_makeYieldBox,      0, 0);
PRIM_FUNC(Io0_flatCwd,           0, 0);
PRIM_FUNC(Io0_flatFileExists,    1, 1);
PRIM_FUNC(Io0_flatReadLink,      1, 1);
PRIM_FUNC(Io0_flatReadFileUtf8,  1, 1);
PRIM_FUNC(Io0_flatWriteFileUtf8, 2, 2);
PRIM_FUNC(Io0_die,               0, 1);
PRIM_FUNC(Io0_note,              1, 1);
