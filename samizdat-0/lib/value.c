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
 * Does most of the work of `coreOrderIs` and `totalOrderIs`.
 */
static bool doOrderIs(zint argCount, const zvalue *args) {
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
PRIM_IMPL(coreOrderIs) {
    requireRange(argCount, 3, 4);

    zvalue arg0 = args[0];
    zvalue arg1 = args[1];
    ztype ztype0 = datType(arg0);
    ztype ztype1 = datType(arg1);

    if (ztype0 != ztype1) {
        die("Mismatched core types.");
    }

    if (ztype0 == DAT_DERIV) {
        zvalue type0 = constTypeOf(arg0);
        if (!datEq(type0, constTypeOf(arg1))) {
            die("Mismatched derived types.");
        } else if (constIsCoreTypeName(type0)) {
            die("Oddball derived value.");
        }
    }

    return doOrderIs(argCount, args) ? arg1 : NULL;
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
PRIM_IMPL(totalOrder) {
    requireExactly(argCount, 2);
    return constIntFromZint(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(totalOrderIs) {
    requireRange(argCount, 3, 4);
    return doOrderIs(argCount, args) ? args[1] : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(typeOf) {
    requireExactly(argCount, 1);
    return constTypeOf(args[0]);
}
