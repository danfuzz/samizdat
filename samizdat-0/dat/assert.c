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
void datAssertBox(zvalue value) {
    pbAssertType(value, DAT_Box);
}

/* Documented in header. */
void datAssertDeriv(zvalue value) {
    pbAssertType(value, DAT_Deriv);
}

/* Documented in header. */
void datAssertList(zvalue value) {
    pbAssertType(value, DAT_List);
}

/* Documented in header. */
void datAssertMap(zvalue value) {
    pbAssertType(value, DAT_Map);
}

/* Documented in header. */
void datAssertMapSize1(zvalue value) {
    datAssertMap(value);
    if (pbSize(value) != 1) {
        die("Not a size 1 map.");
    }
}

/* Documented in header. */
void datAssertUniqlet(zvalue value) {
    pbAssertType(value, DAT_Uniqlet);
}
