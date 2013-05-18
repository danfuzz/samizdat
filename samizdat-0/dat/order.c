/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"


/*
 * Exported functions
 */

/* Documented in header. */
bool datEq(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return true;
    }

    ztype t1 = datType(v1);
    ztype t2 = datType(v2);

    if (t1 != t2) {
        return false;
    }

    zint size1 = datSize(v1);
    zint size2 = datSize(v2);

    if (size1 != size2) {
        return false;
    }

    switch (t1) {
        case DAT_INT:     return datIntEq(v1, v2);
        case DAT_STRING:  return datStringEq(v1, v2);
        case DAT_LIST:    return datListEq(v1, v2);
        case DAT_MAP:     return datMapEq(v1, v2);
        case DAT_UNIQLET: return false;
        case DAT_TOKEN:   return datTokenEq(v1, v2);
    }

    die("Invalid type (shouldn't happen).");
}

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

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
        case DAT_INT:     return datIntOrder(v1, v2);
        case DAT_STRING:  return datStringOrder(v1, v2);
        case DAT_LIST:    return datListOrder(v1, v2);
        case DAT_MAP:     return datMapOrder(v1, v2);
        case DAT_UNIQLET: return datUniqletOrder(v1, v2);
        case DAT_TOKEN:   return datTokenOrder(v1, v2);
    }

    die("Invalid type (shouldn't happen).");
}
