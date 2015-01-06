// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Record.h"

#include "langnode.h"


//
// Exported functions
//

// Documented in spec.
zvalue extractLiteral(zvalue node) {
    return nodeRecTypeIs(node, NODE_literal)
        ? cm_get(node, SYM(value))
        : NULL;
}

// Documented in spec.
zvalue makeLiteral(zvalue value) {
    return cm_new_Record(SYM(literal), SYM(value), value);
}
