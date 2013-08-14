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
 * Private Definitions
 */

/**
 * Calls `stringNth()`, converting the result into a proper zvalue.
 */
static zvalue makeValueStringNth(zvalue string, zint n) {
    zint ch = stringNth(string, n);

    return (ch < 0) ? NULL : stringFromZchar(ch);
}


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(charFromInt) {
    return stringFromZchar(zcharFromInt(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(intFromChar) {
    zvalue string = args[0];
    pbAssertStringSize1(string);

    return intFromZint(stringNth(string, 0));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringAdd) {
    switch (argCount) {
        case 0: {
            return EMPTY_STRING;
        }
        case 1: {
            pbAssertString(args[0]);
            return args[0];
        }
        case 2: {
            return stringAdd(args[0], args[1]);
        }
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        size += pbSize(args[i]);
    }

    zchar chars[size];

    for (zint i = 0, at = 0; i < argCount; i++) {
        zcharsFromString(&chars[at], args[i]);
        at += pbSize(args[i]);
    }

    return stringFromZchars(size, chars);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringGet) {
    return doNthLenient(makeValueStringNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringNth) {
    return doNthStrict(makeValueStringNth, args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringSlice) {
    zvalue string = args[0];
    zint startIndex = zintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? zintFromInt(args[2]) : pbSize(string);

    return stringSlice(string, startIndex, endIndex);
}
