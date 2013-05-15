/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listPutNth) {
    requireExactly(argCount, 3);
    return datListPutNth(args[0], datZintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listInsNth) {
    requireExactly(argCount, 3);
    return datListInsNth(args[0], datZintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listAdd) {
    zvalue result = EMPTY_LIST;

    for (zint i = 0; i < argCount; i++) {
        result = datListAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listNth) {
    return doNth(datListNth, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listDelNth) {
    requireExactly(argCount, 2);

    if (!datTypeIs(args[1], DAT_INT)) {
        return args[0];
    }

    return datListDelNth(args[0], datZintFromInt(args[1]));
}
