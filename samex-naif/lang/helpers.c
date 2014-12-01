// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Map.h"
#include "type/Record.h"
#include "type/SymbolTable.h"

#include "helpers.h"


//
// Module functions
//

// Documented in header.
zvalue recFrom1(zvalue name, zvalue k1, zvalue v1) {
    return cm_new(Record, name, symtabFrom1(k1, v1));
}

// Documented in header.
zvalue recFrom2(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return cm_new(Record, name, symtabFrom2(k1, v1, k2, v2));
}

// Documented in header.
zvalue recFrom3(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return cm_new(Record, name, symtabFrom3(k1, v1, k2, v2, k3, v3));
}

// Documented in header.
zvalue recFrom4(zvalue name, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    return cm_new(Record, name, symtabFrom4(k1, v1, k2, v2, k3, v3, k4, v4));
}

// Documented in header.
zvalue symtabFrom1(zvalue k1, zvalue v1) {
    return symtabFrom4(k1, v1, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue symtabFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return symtabFrom4(k1, v1, k2, v2, NULL, NULL, NULL, NULL);
}

// Documented in header.
zvalue symtabFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return symtabFrom4(k1, v1, k2, v2, k3, v3, NULL, NULL);
}

// Documented in header.
zvalue symtabFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
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
