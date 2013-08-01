/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


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
            return datListAdd(args[0], args[1]);
        }
    }

    // The general case. This isn't particularly fancy, because in practice
    // (as of this writing) it barely ever gets used.

    zvalue result = args[0];

    for (zint i = 1; i < argCount; i++) {
        result = datListAdd(result, args[i]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listDelNth) {
    requireExactly(argCount, 2);

    if (!datCoreTypeIs(args[1], DAT_Int)) {
        return args[0];
    }

    return datListDelNth(args[0], datZintFromInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listFilter) {
    requireExactly(argCount, 2);

    zvalue function = args[0];
    zvalue list = args[1];
    zint size = datSize(list);
    zvalue result[size];
    zint at = 0;

    datAssertList(list);

    for (zint i = 0; i < size; i++) {
        zvalue elem = datListNth(list, i);
        zvalue one = datFnCall(function, 1, &elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return datListFromArray(at, result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listGet) {
    requireExactly(argCount, 2);
    return doNthLenient(datListNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listInsNth) {
    requireExactly(argCount, 3);
    return datListInsNth(args[0], datZintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listNth) {
    requireExactly(argCount, 2);
    return doNthStrict(datListNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listPutNth) {
    requireExactly(argCount, 3);
    return datListPutNth(args[0], datZintFromInt(args[1]), args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listReverse) {
    requireExactly(argCount, 1);

    zvalue list = args[0];
    zint size = datSize(list);
    zvalue elems[size];

    datArrayFromList(elems, list);

    for (zint low = 0, high = size - 1; low < high; low++, high--) {
        zvalue temp = elems[low];
        elems[low] = elems[high];
        elems[high] = temp;
    }

    return datListFromArray(size, elems);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listSlice) {
    requireAtLeast(argCount, 2);

    zvalue list = args[0];
    zint startIndex = datZintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? datZintFromInt(args[2]) : datSize(list);

    return datListSlice(list, startIndex, endIndex);
}
