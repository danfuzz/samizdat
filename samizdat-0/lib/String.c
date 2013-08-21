/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"
#include "util.h"


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

    return intFromZint(zcharFromString(string));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringCat) {
    if (argCount == 0) {
        return EMPTY_STRING;
    } else {
        return funCall(GFN_cat, argCount, args);
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringSlice) {
    zvalue string = args[0];
    zint startIndex = zintFromInt(args[1]);
    zint endIndex = (argCount == 3) ? zintFromInt(args[2]) : collSize(string);

    return stringSlice(string, startIndex, endIndex);
}
