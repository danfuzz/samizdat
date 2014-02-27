/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Function.h"
#include "type/Generator.h"
#include "type/List.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(catCollect) {
    // Handle some easy / special cases up-front.
    switch (argCount) {
        case 0: {
            return EMPTY_LIST;
        }
        case 1: {
            return GFN_CALL(collect, args[0]);
        }
    }

    zvalue collected[argCount];
    for (zint i = 0; i < argCount; i++) {
        collected[i] = GFN_CALL(collect, args[i]);
    }

    return funCall(GFN_cat, argCount, collected);
}

/* Documented in spec. */
FUN_IMPL_DECL(makeList) {
    return listFromArray(argCount, args);
}
