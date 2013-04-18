/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Exported functions
 */

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    if (v1 == v2) {
        return ZSAME;
    }

    ztype t1 = datType(v1);
    ztype t2 = datType(v2);

    if (t1 < t2) {
        return ZLESS;
    } else if (t1 > t2) {
        return ZMORE;
    }

    switch (t1) {
        case DAT_INTLET:    return datIntletOrder(v1, v2);
        case DAT_STRINGLET: return datStringletOrder(v1, v2);
        case DAT_LISTLET:   return datListletOrder(v1, v2);
        case DAT_MAPLET:    return datMapletOrder(v1, v2);
        case DAT_UNIQLET:   return datUniqletOrder(v1, v2);
    }

    die("Invalid type (shouldn't happen).");
}
