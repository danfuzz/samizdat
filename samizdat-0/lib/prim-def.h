/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `PRIM_FUNC(name)` defines a primitive function with the given name.
 *
 * **Note:** This file gets `#include`d multiple times, and so does not
 * have the usual guard macros.
 */

PRIM_FUNC(apply);
PRIM_FUNC(argsMap);
PRIM_FUNC(argsReduce);
PRIM_FUNC(boxGet);
PRIM_FUNC(boxIsSet);
PRIM_FUNC(boxSet);
PRIM_FUNC(iadd);
PRIM_FUNC(iand);
PRIM_FUNC(idiv);
PRIM_FUNC(ifTrue);
PRIM_FUNC(ifValue);
PRIM_FUNC(imod);
PRIM_FUNC(imul);
PRIM_FUNC(ineg);
PRIM_FUNC(inot);
PRIM_FUNC(intFromString);
PRIM_FUNC(io0FlatCwd);
PRIM_FUNC(io0FlatReadLink);
PRIM_FUNC(io0FlatReadFileUtf8);
PRIM_FUNC(io0FlatWriteFileUtf8);
PRIM_FUNC(io0Die);
PRIM_FUNC(io0Note);
PRIM_FUNC(ior);
PRIM_FUNC(irem);
PRIM_FUNC(ishl);
PRIM_FUNC(ishr);
PRIM_FUNC(isub);
PRIM_FUNC(ixor);
PRIM_FUNC(listAdd);
PRIM_FUNC(listDelNth);
PRIM_FUNC(listInsNth);
PRIM_FUNC(listNth);
PRIM_FUNC(listPutNth);
PRIM_FUNC(listReverse);
PRIM_FUNC(listSlice);
PRIM_FUNC(loop);
PRIM_FUNC(loopReduce);
PRIM_FUNC(lowOrder);
PRIM_FUNC(lowOrderIs);
PRIM_FUNC(lowSize);
PRIM_FUNC(lowType);
PRIM_FUNC(makeToken);
PRIM_FUNC(makeList);
PRIM_FUNC(makeMap);
PRIM_FUNC(makeMapReversed);
PRIM_FUNC(makeRange);
PRIM_FUNC(makeUniqlet);
PRIM_FUNC(mapAdd);
PRIM_FUNC(mapDel);
PRIM_FUNC(mapGet);
PRIM_FUNC(mapHasKey);
PRIM_FUNC(mapKey);
PRIM_FUNC(mapNth);
PRIM_FUNC(mapPut);
PRIM_FUNC(mapValue);
PRIM_FUNC(mutableBox);
PRIM_FUNC(nonlocalExit);
PRIM_FUNC(object);
PRIM_FUNC(sam0Eval);
PRIM_FUNC(sam0Tokenize);
PRIM_FUNC(sam0Tree);
PRIM_FUNC(stringAdd);
PRIM_FUNC(stringFromInt);
PRIM_FUNC(stringNth);
PRIM_FUNC(stringSlice);
PRIM_FUNC(tokenHasValue);
PRIM_FUNC(tokenType);
PRIM_FUNC(tokenValue);
PRIM_FUNC(yieldBox);
