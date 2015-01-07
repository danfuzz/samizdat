// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Cmp.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue cmpEq(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `cmpEq`.");
    } else if (value == other) {
        return value;
    } else if (haveSameClass(value, other)) {
        return (METH_CALL(value, crossEq, other) == NULL) ? NULL : value;
    } else {
        return NULL;
    }
}

// Documented in header.
bool cmpEqNullOk(zvalue value, zvalue other) {
    if (value == other) {
        return true;
    } else if ((value == NULL) || (other == NULL)) {
        return false;
    } else {
        return cmpEq(value, other) != NULL;
    }
}

// Documented in header.
zvalue cmpOrder(zvalue value, zvalue other) {
    if ((value == NULL) || (other == NULL)) {
        die("Shouldn't happen: NULL argument passed to `cmpOrder`.");
    } else if (value == other) {
        return SYM(same);
    } else if (haveSameClass(value, other)) {
        return METH_CALL(value, crossOrder, other);
    } else {
        return METH_CALL(classOf(value), perOrder, classOf(other));
    }
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_2(Cmp, eq, value, other) {
    return cmpEq(value, other);
}

// Documented in spec.
CMETH_IMPL_2(Cmp, ge, value, other) {
    if (value == other) {
        return value;
    } else {
        zvalue order = cmpOrder(value, other);
        return ((order != NULL) && (zorderFromSymbol(order) != ZLESS))
            ? value : NULL;
    }
}

// Documented in spec.
CMETH_IMPL_2(Cmp, gt, value, other) {
    if (value == other) {
        return NULL;
    } else {
        zvalue order = cmpOrder(value, other);
        return ((order != NULL) && (zorderFromSymbol(order) == ZMORE))
            ? value : NULL;
    }
}

// Documented in spec.
CMETH_IMPL_2(Cmp, le, value, other) {
    if (value == other) {
        return value;
    } else {
        zvalue order = cmpOrder(value, other);
        return ((order != NULL) && (zorderFromSymbol(order) != ZMORE))
            ? value : NULL;
    }
}

// Documented in spec.
CMETH_IMPL_2(Cmp, lt, value, other) {
    if (value == other) {
        return NULL;
    } else {
        zvalue order = cmpOrder(value, other);
        return ((order != NULL) && (zorderFromSymbol(order) == ZLESS))
            ? value : NULL;
    }
}

// Documented in spec.
CMETH_IMPL_2(Cmp, ne, value, other) {
    return (cmpEq(value, other) == NULL) ? value : NULL;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, order, value, other) {
    return cmpOrder(value, other);
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perEq, value, other) {
    zvalue result = METH_CALL(value, perEq, other);
    return (result == NULL) ? NULL : value;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perGe, value, other) {
    zvalue order = METH_CALL(value, perOrder, other);
    return ((order != NULL) && (zorderFromSymbol(order) != ZLESS))
        ? value : NULL;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perGt, value, other) {
    zvalue order = METH_CALL(value, perOrder, other);
    return ((order != NULL) && (zorderFromSymbol(order) == ZMORE))
        ? value : NULL;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perLe, value, other) {
    zvalue order = METH_CALL(value, perOrder, other);
    return ((order != NULL) && (zorderFromSymbol(order) != ZMORE))
        ? value : NULL;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perLt, value, other) {
    zvalue order = METH_CALL(value, perOrder, other);
    return ((order != NULL) && (zorderFromSymbol(order) == ZLESS))
        ? value : NULL;
}

// Documented in spec.
CMETH_IMPL_2(Cmp, perNe, value, other) {
    zvalue result = METH_CALL(value, perEq, other);
    return (result == NULL) ? value : NULL;
}

/** Initializes the module. */
MOD_INIT(Cmp) {
    MOD_USE(Core);

    CLS_Cmp = makeCoreClass(SYM(Cmp), CLS_Core,
        METH_TABLE(
            CMETH_BIND(Cmp, eq),
            CMETH_BIND(Cmp, ge),
            CMETH_BIND(Cmp, gt),
            CMETH_BIND(Cmp, le),
            CMETH_BIND(Cmp, lt),
            CMETH_BIND(Cmp, ne),
            CMETH_BIND(Cmp, order),
            CMETH_BIND(Cmp, perEq),
            CMETH_BIND(Cmp, perGe),
            CMETH_BIND(Cmp, perGt),
            CMETH_BIND(Cmp, perLe),
            CMETH_BIND(Cmp, perLt),
            CMETH_BIND(Cmp, perNe)),
        NULL);
}

// Documented in header.
zvalue CLS_Cmp = NULL;
