/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Does most of the work of `coreOrderIs`.
 */
static bool doCoreOrderIs(zint argCount, const zvalue *args) {
    zorder want = datZintFromInt(args[2]);

    if ((argCount == 3) && (want == ZSAME)) {
        return datEq(args[0], args[1]);
    }

    zorder comp = datOrder(args[0], args[1]);

    return (comp == want) ||
        ((argCount == 4) && (comp == datZintFromInt(args[3])));
}


/*
 * Exported functions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(coreOrder) {
    requireExactly(argCount, 2);
    return constIntFromZint(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(coreOrderIs) {
    requireRange(argCount, 3, 4);
    return doCoreOrderIs(argCount, args) ? args[1] : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(coreSizeOf) {
    requireExactly(argCount, 1);
    return constIntFromZint(datSize(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(dataOf) {
    requireExactly(argCount, 1);
    return constDataOf(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(isCoreValue) {
    requireExactly(argCount, 1);

    zvalue value = args[0];
    return (datType(value) != DAT_DERIV) ? value : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(typeOf) {
    requireExactly(argCount, 1);
    return constTypeOf(args[0]);
}
