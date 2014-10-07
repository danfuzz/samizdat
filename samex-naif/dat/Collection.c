// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Collection values
//

#include "type/Collection.h"


//
// Exported Definitions
//

// Documented in header.
zvalue collPut(zvalue coll, zvalue key, zvalue value) {
    return METH_CALL(put, coll, key, value);
}
