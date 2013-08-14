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

PRIM_DEF(boxCanStore, GFN_canStore);
PRIM_DEF(boxFetch,    GFN_fetch);
PRIM_DEF(boxStore,    GFN_store);

PRIM_DEF(call,       GFN_call);
PRIM_DEF(canCall,    GFN_canCall);
PRIM_DEF(coreSizeOf, GFN_size);
PRIM_DEF(nullBox,    DAT_NULL_BOX);

PRIM_FUNC(charFromInt,    1, 1);
PRIM_FUNC(coreOrder,      2, 2);
PRIM_FUNC(coreOrderIs,    3, 4);
PRIM_FUNC(dataOf,         1, 2);
PRIM_FUNC(iabs,           1, 1);
PRIM_FUNC(iadd,           2, 2);
PRIM_FUNC(iand,           2, 2);
PRIM_FUNC(ibit,           2, 2);
PRIM_FUNC(idiv,           2, 2);
PRIM_FUNC(ifIs,           2, 3);
PRIM_FUNC(ifNot,          2, 2);
PRIM_FUNC(ifValue,        2, 3);
PRIM_FUNC(ifValueOr,      2, 2);
PRIM_FUNC(imod,           2, 2);
PRIM_FUNC(imul,           2, 2);
PRIM_FUNC(ineg,           1, 1);
PRIM_FUNC(inot,           1, 1);
PRIM_FUNC(intFromChar,    1, 1);
PRIM_FUNC(intGet,         2, 2);
PRIM_FUNC(intNth,         2, 2);
PRIM_FUNC(io0FlatCwd,     0, 0);
PRIM_FUNC(io0FlatReadLink,      1, 1);
PRIM_FUNC(io0FlatReadFileUtf8,  1, 1);
PRIM_FUNC(io0FlatWriteFileUtf8, 2, 2);
PRIM_FUNC(io0Die,         0, 1);
PRIM_FUNC(io0Note,        1, 1);
PRIM_FUNC(ior,            2, 2);
PRIM_FUNC(irem,           2, 2);
PRIM_FUNC(isOpaqueValue,  1, 1);
PRIM_FUNC(isign,          1, 1);
PRIM_FUNC(ishl,           2, 2);
PRIM_FUNC(ishr,           2, 2);
PRIM_FUNC(isub,           2, 2);
PRIM_FUNC(ixor,           2, 2);
PRIM_FUNC(listAdd,        0, -1);
PRIM_FUNC(listDelNth,     2, 2);
PRIM_FUNC(listFilter,     2, 2);
PRIM_FUNC(listGet,        2, 2);
PRIM_FUNC(listInsNth,     3, 3);
PRIM_FUNC(listNth,        2, 2);
PRIM_FUNC(listPutNth,     3, 3);
PRIM_FUNC(listReverse,    1, 1);
PRIM_FUNC(listSlice,      2, 3);
PRIM_FUNC(loop,           1, 1);
PRIM_FUNC(loopReduce,     1, -1);
PRIM_FUNC(makeList,       0, -1);
PRIM_FUNC(makeMapping,    1, -1);
PRIM_FUNC(makeMutableBox, 0, 1);
PRIM_FUNC(makeUniqlet,    0, 0);
PRIM_FUNC(makeValue,      1, 2);
PRIM_FUNC(makeYieldBox,   0, 0);
PRIM_FUNC(mapAdd,         0, -1);
PRIM_FUNC(mapDel,         1, -1);
PRIM_FUNC(mapGet,         2, 2);
PRIM_FUNC(mapKeys,        1, 1);
PRIM_FUNC(mapNth,         2, 2);
PRIM_FUNC(mapPut,         3, 3);
PRIM_FUNC(mappingKey,     1, 1);
PRIM_FUNC(mappingValue,   1, 1);
PRIM_FUNC(nonlocalExit,   1, 2);
PRIM_FUNC(sam0Eval,       2, 2);
PRIM_FUNC(sam0Tokenize,   1, 1);
PRIM_FUNC(sam0Tree,       1, 1);
PRIM_FUNC(stringAdd,      0, -1);
PRIM_FUNC(stringGet,      2, 2);
PRIM_FUNC(stringNth,      2, 2);
PRIM_FUNC(stringSlice,    2, 3);
PRIM_FUNC(totalOrder,     2, 2);
PRIM_FUNC(totalOrderIs,   3, 4);
PRIM_FUNC(typeName,       1, 1);
PRIM_FUNC(typeOf,         1, 1);
