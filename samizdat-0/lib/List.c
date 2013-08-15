/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listAdd) {
    // Special case code for the common and easy cases.
    switch (argCount) {
        case 0: {
            return EMPTY_LIST;
        }
        case 1: {
            datAssertList(args[0]);
            return args[0];
        }
        case 2: {
            return listAdd(args[0], args[1]);
        }
    }

    // The general case. This isn't particularly fancy, because in practice
    // (as of this writing) it barely ever gets used.

    zvalue result = args[0];

    for (zint i = 1; i < argCount; i++) {
        result = listAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listDelNth) {
    if (!hasType(args[1], TYPE_Int)) {
        return args[0];
    }

    return listDelNth(args[0], zintFromInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listFilter) {
    zvalue function = args[0];
    zvalue list = args[1];
    zint size = pbSize(list);
    zvalue result[size];
    zint at = 0;

    datAssertList(list);

    for (zint i = 0; i < size; i++) {
        zvalue elem = listNth(list, i);
        zvalue one = FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listGet) {
   return doNthLenient(listNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listInsNth) {
    return listInsNth(args[0], zintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listNth) {
    return doNthStrict(listNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listPutNth) {
    return listPutNth(args[0], zintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listReverse) {
    zvalue list = args[0];
    zint size = pbSize(list);
    zvalue elems[size];

    arrayFromList(elems, list);

    for (zint low = 0, high = size - 1; low < high; low++, high--) {
        zvalue temp = elems[low];
        elems[low] = elems[high];
        elems[high] = temp;
    }

    return listFromArray(size, elems);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listSlice) {
    zvalue list = args[0];
    zint startIndex = zintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? zintFromInt(args[2]) : pbSize(list);

    return listSlice(list, startIndex, endIndex);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeList) {
    return listFromArray(argCount, args);
}
