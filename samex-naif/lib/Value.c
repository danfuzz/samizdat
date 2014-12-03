// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Cmp.h"

#include "impl.h"
#include "util.h"


//
// Private Definitions
//

/**
 * Common implementation for `cast` and `maybeCast`.
 */
static zvalue doCast(zvalue cls, zvalue value) {
    if (METH_CALL(cls, accepts, value)) {
        return value;
    }

    zvalue result = METH_CALL(value, castToward, cls);

    if (result != NULL) {
        if (METH_CALL(cls, accepts, result)) {
            return result;
        }
        value = result;
    }

    result = METH_CALL(cls, castFrom, value);

    if ((result != NULL) && METH_CALL(cls, accepts, result)) {
        return result;
    }

    return NULL;
}


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(cast) {
    zvalue cls = args.elems[0];
    zvalue value = args.elems[1];
    zvalue result = doCast(cls, value);

    if (result != NULL) {
        return result;
    }

    die("Could not cast: (%s) %s", cm_debugString(cls), cm_debugString(value));
}

// Documented in spec.
FUN_IMPL_DECL(eq) {
    zvalue value = args.elems[0];
    zvalue other = args.elems[1];

    return cmpEq(value, other);
}

// Documented in spec.
FUN_IMPL_DECL(classOf) {
    return classOf(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(maybeCast) {
    zvalue cls = args.elems[0];
    zvalue value = args.elems[1];

    return doCast(cls, value);
}

// Documented in spec.
FUN_IMPL_DECL(order) {
    zvalue value = args.elems[0];
    zvalue other = args.elems[1];

    return valOrder(value, other);
}
