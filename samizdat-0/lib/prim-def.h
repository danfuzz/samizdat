/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Note: This file gets `#include`d multiple times, and so does not
 * have the usual guard macros.
 *
 * `PRIM_FUNC(name)` defines a primitive function with the given name.
 */

PRIM_FUNC(apply);
PRIM_FUNC(highletType);
PRIM_FUNC(highletValue);
PRIM_FUNC(iadd);
PRIM_FUNC(iand);
PRIM_FUNC(idiv);
PRIM_FUNC(ifTrue);
PRIM_FUNC(ifValue);
PRIM_FUNC(imod);
PRIM_FUNC(imul);
PRIM_FUNC(ineg);
PRIM_FUNC(inot);
PRIM_FUNC(intletFromStringlet);
PRIM_FUNC(io0Die);
PRIM_FUNC(io0Note);
PRIM_FUNC(io0PathFromStringlet);
PRIM_FUNC(io0ReadFileUtf8);
PRIM_FUNC(io0WriteFileUtf8);
PRIM_FUNC(ior);
PRIM_FUNC(irem);
PRIM_FUNC(ishl);
PRIM_FUNC(ishr);
PRIM_FUNC(isub);
PRIM_FUNC(ixor);
PRIM_FUNC(listletAdd);
PRIM_FUNC(listletDelNth);
PRIM_FUNC(listletInsNth);
PRIM_FUNC(listletNth);
PRIM_FUNC(listletPutNth);
PRIM_FUNC(lowOrder);
PRIM_FUNC(lowOrderIs);
PRIM_FUNC(lowSize);
PRIM_FUNC(lowType);
PRIM_FUNC(makeHighlet);
PRIM_FUNC(makeListlet);
PRIM_FUNC(makeMaplet);
PRIM_FUNC(makeUniqlet);
PRIM_FUNC(mapletAdd);
PRIM_FUNC(mapletDel);
PRIM_FUNC(mapletGet);
PRIM_FUNC(mapletKeys);
PRIM_FUNC(mapletPut);
PRIM_FUNC(sam0Eval);
PRIM_FUNC(sam0Tree);
PRIM_FUNC(stringletAdd);
PRIM_FUNC(stringletFromIntlet);
PRIM_FUNC(stringletNth);
