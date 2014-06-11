// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "type/Map.h"

#include "helpers.h"


//
// Module functions
//

// Documented in header.
zvalue mapFrom1(zvalue k1, zvalue v1) {
    return mapFrom4(k1, v1, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return mapFrom4(k1, v1, k2, v2, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue mapFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return mapFrom4(k1, v1, k2, v2, k3, v3, NULL, NULL);
}

// Documented in header.
zvalue mapFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    zmapping elems[4];
    zint at = 0;

    if (v1 != NULL) { elems[at].key = k1; elems[at].value = v1; at++; }
    if (v2 != NULL) { elems[at].key = k2; elems[at].value = v2; at++; }
    if (v3 != NULL) { elems[at].key = k3; elems[at].value = v3; at++; }
    if (v4 != NULL) { elems[at].key = k4; elems[at].value = v4; at++; }

    return (at == 0) ? EMPTY_MAP : mapFromArray(at, elems);
}

// Documented in header.
zvalue listFrom1(zvalue e1) {
    return listFromArray(1, &e1);
}

// Documented in header.
zvalue listFrom2(zvalue e1, zvalue e2) {
    zvalue elems[2] = { e1, e2 };
    return listFromArray(2, elems);
}

// Documented in header.
zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3) {
    zvalue elems[3] = { e1, e2, e3 };
    return listFromArray(3, elems);
}

// Documented in header.
zvalue listAppend(zvalue list, zvalue elem) {
    return GFN_CALL(cat, list, listFrom1(elem));
}
