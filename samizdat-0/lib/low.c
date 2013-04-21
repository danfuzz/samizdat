/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "cst.h"
#include "util.h"


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrder) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrderIs) {
    requireRange(argCount, 3, 4);

    zorder comp = datOrder(args[0], args[1]);
    bool result =
        (comp == datIntFromIntlet(args[2])) ||
        ((argCount == 4) && (comp == datIntFromIntlet(args[3])));

    return langBooleanFromBool(result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowSize) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowType) {
    requireExactly(argCount, 1);

    switch (datType(args[0])) {
        case DAT_INTLET:    return CST_STR_INTLET;
        case DAT_STRINGLET: return CST_STR_STRINGLET;
        case DAT_LISTLET:   return CST_STR_LISTLET;
        case DAT_MAPLET:    return CST_STR_MAPLET;
        case DAT_UNIQLET:   return CST_STR_UNIQLET;
        case DAT_HIGHLET:   return CST_STR_HIGHLET;
        default: {
            die("Invalid value type (shouldn't happen): %d", datType(args[0]));
        }
    }
}
