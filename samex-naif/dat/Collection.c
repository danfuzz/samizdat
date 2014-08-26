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

    SYM_INIT(del);
    SYM_INIT(keyList);
    SYM_INIT(nthMapping);
    SYM_INIT(put);
    SYM_INIT(valueList);
}

// Documented in header.
SYM_DEF(del);

// Documented in header.
SYM_DEF(keyList);

// Documented in header.
SYM_DEF(nthMapping);

// Documented in header.
SYM_DEF(put);

// Documented in header.
SYM_DEF(valueList);
