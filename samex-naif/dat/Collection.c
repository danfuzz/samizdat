// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Collection values
//

#include "type/Collection.h"
#include "type/String.h"
#include "type/Class.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue collDel(zvalue coll, zvalue key) {
    return GFN_CALL(del, coll, key);
}

// Documented in header.
zvalue collPut(zvalue coll, zvalue key, zvalue value) {
    return GFN_CALL(put, coll, key, value);
}


//
// Type Definition: `Collection`
//

/** Initializes the module. */
MOD_INIT(Collection) {
    MOD_USE(OneOff);

    GFN_del = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "del"));
    datImmortalize(GFN_del);

    GFN_keyList = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "keyList"));
    datImmortalize(GFN_keyList);

    GFN_nthMapping =
        makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nthMapping"));
    datImmortalize(GFN_nthMapping);

    GFN_put = makeGeneric(3, 3, GFN_NONE, stringFromUtf8(-1, "put"));
    datImmortalize(GFN_put);

    GFN_valueList =
        makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "valueList"));
    datImmortalize(GFN_valueList);
}

// Documented in header.
zvalue GFN_del = NULL;

// Documented in header.
zvalue GFN_keyList = NULL;

// Documented in header.
zvalue GFN_nthMapping = NULL;

// Documented in header.
zvalue GFN_put = NULL;

// Documented in header.
zvalue GFN_valueList = NULL;
