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

    SEL_INIT(2, 2, del);

    SEL_INIT(1, 1, keyList);

    SEL_INIT(2, 2, nthMapping);

    SEL_INIT(3, 3, put);

    SEL_INIT(1, 1, valueList);
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
