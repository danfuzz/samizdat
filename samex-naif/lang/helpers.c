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
zvalue listFrom1(zvalue e1) {
    return (e1 == NULL) ? EMPTY_LIST : listFromValue(e1);
}

// Documented in header.
zvalue listFrom2(zvalue e1, zvalue e2) {
    return listFrom4(e1, e2, NULL, NULL);
}

// Documented in header.
zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3) {
    return listFrom4(e1, e2, e3, NULL);
}

// Documented in header.
zvalue listFrom4(zvalue e1, zvalue e2, zvalue e3, zvalue e4) {
    zvalue elems[4];
    zint at = 0;

    if (e1 != NULL) { elems[at] = e1; at++; }
    if (e2 != NULL) { elems[at] = e2; at++; }
    if (e3 != NULL) { elems[at] = e3; at++; }
    if (e4 != NULL) { elems[at] = e4; at++; }

    return (at == 0) ? EMPTY_LIST : listFromZarray((zarray) {at, elems});
}

// Documented in header.
zvalue listAppend(zvalue list, zvalue elem) {
    return cm_cat(list, listFrom1(elem));
}

// Documented in header.
zvalue listPrepend(zvalue elem, zvalue list) {
    return cm_cat(listFrom1(elem), list);
}

// Documented in header.
zvalue mapFrom1(zvalue k1, zvalue v1) {
    return (v1 == NULL)
        ? EMPTY_MAP
        : mapFromMapping((zmapping) {k1, v1});
}

// Documented in header.
zvalue recordFrom1(zvalue name, zvalue k1, zvalue v1) {
    return cm_new(Record, name, tableFrom1(k1, v1));
}

// Documented in header.
zvalue recordFrom2(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return cm_new(Record, name, tableFrom2(k1, v1, k2, v2));
}

// Documented in header.
zvalue recordFrom3(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return cm_new(Record, name, tableFrom3(k1, v1, k2, v2, k3, v3));
}

// Documented in header.
zvalue recordFrom4(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    return cm_new(Record, name, tableFrom4(k1, v1, k2, v2, k3, v3, k4, v4));
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

    return (at == 0)
        ? EMPTY_SYMBOL_TABLE
        : symtabFromZassoc((zassoc) {at, elems});
}
