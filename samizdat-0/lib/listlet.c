/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletPutNth) {
    requireExactly(argCount, 3);
    return datListletPutNth(args[0], datIntFromIntlet(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletInsNth) {
    requireExactly(argCount, 3);
    return datListletInsNth(args[0], datIntFromIntlet(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAdd) {
    zvalue result = EMPTY_LISTLET;

    for (zint i = 0; i < argCount; i++) {
        result = datListletAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletNth) {
    return doNth(datListletNth, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletDelNth) {
    requireExactly(argCount, 2);

    if (!datTypeIs(args[1], DAT_INTLET)) {
        return args[0];
    }

    return datListletDelNth(args[0], datIntFromIntlet(args[1]));
}
