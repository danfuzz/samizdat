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
PRIM_DEF(Box,        TYPE_Box);
PRIM_DEF(Function,   TYPE_Function);
PRIM_DEF(Generic,    TYPE_Generic);
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

// Generic functions: `Box` protocol
PRIM_DEF(canStore,   GFN_canStore);
PRIM_DEF(fetch,      GFN_fetch);
PRIM_DEF(store,      GFN_store);

// Generic functions: `Callable` protocol
PRIM_DEF(call,       GFN_call);
PRIM_DEF(canCall,    GFN_canCall);

// Generic functions: `Collection` protocol and sub-protocols
PRIM_DEF(cat,        GFN_cat);
PRIM_DEF(get,        GFN_get);
PRIM_DEF(nth,        GFN_nth);
PRIM_DEF(size,       GFN_size);
PRIM_DEF(slice,      GFN_slice);

// Generic functions: `Int` protocol
PRIM_DEF(abs,        GFN_abs);
PRIM_DEF(add,        GFN_add);
PRIM_DEF(and,        GFN_and);
PRIM_DEF(bit,        GFN_bit);
PRIM_DEF(div,        GFN_div);
PRIM_DEF(divEu,      GFN_divEu);
PRIM_DEF(mod,        GFN_mod);
PRIM_DEF(modEu,      GFN_modEu);
PRIM_DEF(mul,        GFN_mul);
PRIM_DEF(neg,        GFN_neg);
PRIM_DEF(not,        GFN_not);
PRIM_DEF(or,         GFN_or);
PRIM_DEF(shl,        GFN_shl);
PRIM_DEF(shr,        GFN_shr);
PRIM_DEF(sign,       GFN_sign);
PRIM_DEF(sub,        GFN_sub);
PRIM_DEF(xor,        GFN_xor);

// Special values
PRIM_DEF(nullBox,    DAT_NULL_BOX);

// Primitive functions
PRIM_FUNC(charFromInt,    1, 1);
PRIM_FUNC(dataOf,         1, 2);
PRIM_FUNC(eq,             2, 2);
PRIM_FUNC(ge,             2, 2);
PRIM_FUNC(gt,             2, 2);
PRIM_FUNC(hasType,        2, 2);
PRIM_FUNC(ifIs,           2, 3);
PRIM_FUNC(ifNot,          2, 2);
PRIM_FUNC(ifValue,        2, 3);
PRIM_FUNC(ifValueOr,      2, 2);
PRIM_FUNC(intFromChar,    1, 1);
PRIM_FUNC(io0FlatCwd,     0, 0);
PRIM_FUNC(io0FlatReadLink,      1, 1);
PRIM_FUNC(io0FlatReadFileUtf8,  1, 1);
PRIM_FUNC(io0FlatWriteFileUtf8, 2, 2);
PRIM_FUNC(io0Die,         0, 1);
PRIM_FUNC(io0Note,        1, 1);
PRIM_FUNC(isOpaqueValue,  1, 1);
PRIM_FUNC(le,             2, 2);
PRIM_FUNC(listDelNth,     2, 2);
PRIM_FUNC(listFilter,     2, 2);
PRIM_FUNC(listInsNth,     3, 3);
PRIM_FUNC(listPutNth,     3, 3);
PRIM_FUNC(listReverse,    1, 1);
PRIM_FUNC(loop,           1, 1);
PRIM_FUNC(loopReduce,     1, -1);
PRIM_FUNC(lt,             2, 2);
PRIM_FUNC(makeList,       0, -1);
PRIM_FUNC(makeMutableBox, 0, 1);
PRIM_FUNC(makeUniqlet,    0, 0);
PRIM_FUNC(makeValue,      1, 2);
PRIM_FUNC(makeValueMap,   1, -1);
PRIM_FUNC(makeYieldBox,   0, 0);
PRIM_FUNC(mapDel,         1, -1);
PRIM_FUNC(mapKeys,        1, 1);
PRIM_FUNC(mapPut,         3, 3);
PRIM_FUNC(mappingKey,     1, 1);
PRIM_FUNC(mappingValue,   1, 1);
PRIM_FUNC(ne,             2, 2);
PRIM_FUNC(nonlocalExit,   1, 2);
PRIM_FUNC(optValue,       1, 1);
PRIM_FUNC(sam0Eval,       2, 2);
PRIM_FUNC(sam0Tokenize,   1, 1);
PRIM_FUNC(sam0Tree,       1, 1);
PRIM_FUNC(totalOrder,     2, 2);
PRIM_FUNC(typeName,       1, 1);
PRIM_FUNC(typeOf,         1, 1);
