// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "type/Map.h"
#include "type/Record.h"
#include "type/SymbolTable.h"

#include "helpers.h"


//
// Module functions
//

// Documented in header.
zvalue derivFrom1(zvalue cls, zvalue k1, zvalue v1) {
    return derivFrom4(cls, k1, v1, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue derivFrom2(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return derivFrom4(cls, k1, v1, k2, v2, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue derivFrom3(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return derivFrom4(cls, k1, v1, k2, v2, k3, v3, NULL, NULL);
}

// Documented in header.
zvalue derivFrom4(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    return makeRecord(cls, tableFrom4(k1, v1, k2, v2, k3, v3, k4, v4));
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
zvalue listFrom4(zvalue e1, zvalue e2, zvalue e3, zvalue e4) {
    zvalue elems[4] = { e1, e2, e3, e4 };
    return listFromArray(4, elems);
}

// Documented in header.
zvalue listAppend(zvalue list, zvalue elem) {
    return METH_CALL(cat, list, listFrom1(elem));
}

// Documented in header.
zvalue listPrepend(zvalue elem, zvalue list) {
    return METH_CALL(cat, listFrom1(elem), list);
}

// Documented in header.
zvalue tableFrom1(zvalue k1, zvalue v1) {
    return tableFrom4(k1, v1, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue tableFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return tableFrom4(k1, v1, k2, v2, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue tableFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return tableFrom4(k1, v1, k2, v2, k3, v3, NULL, NULL);
}

// Documented in header.
zvalue tableFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    zmapping elems[4];
    zint at = 0;

    if (v1 != NULL) { elems[at].key = k1; elems[at].value = v1; at++; }
    if (v2 != NULL) { elems[at].key = k2; elems[at].value = v2; at++; }
    if (v3 != NULL) { elems[at].key = k3; elems[at].value = v3; at++; }
    if (v4 != NULL) { elems[at].key = k4; elems[at].value = v4; at++; }

    return (at == 0) ? EMPTY_SYMBOL_TABLE : symbolTableFromArray(at, elems);
}
