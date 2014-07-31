// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Collection values
//

#include "type/Collection.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue collDel(zvalue coll, zvalue key) {
    return METH_CALL(del, coll, key);
}

// Documented in header.
zvalue collPut(zvalue coll, zvalue key, zvalue value) {
    return METH_CALL(put, coll, key, value);
}


//
// Class Definition: `Collection`
//

/** Initializes the module. */
MOD_INIT(Collection) {
    MOD_USE(OneOff);

    GFN_del = makeGeneric(2, 2, stringFromUtf8(-1, "del"));
    datImmortalize(GFN_del);

    GFN_keyList = makeGeneric(1, 1, stringFromUtf8(-1, "keyList"));
    datImmortalize(GFN_keyList);

    GFN_nthMapping = makeGeneric(2, 2, stringFromUtf8(-1, "nthMapping"));
    datImmortalize(GFN_nthMapping);

    GFN_put = makeGeneric(3, 3, stringFromUtf8(-1, "put"));
    datImmortalize(GFN_put);

    GFN_valueList = makeGeneric(1, 1, stringFromUtf8(-1, "valueList"));
    datImmortalize(GFN_valueList);
}

// Documented in header.
SEL_DEF(del);

// Documented in header.
SEL_DEF(keyList);

// Documented in header.
SEL_DEF(nthMapping);

// Documented in header.
SEL_DEF(put);

// Documented in header.
SEL_DEF(valueList);
