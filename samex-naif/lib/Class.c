// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Class.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(makeDerivedDataType) {
    return makeDerivedDataType(args[0]);
}

// Documented in spec.
FUN_IMPL_DECL(typeName) {
    return typeName(args[0]);
}

// Documented in spec.
FUN_IMPL_DECL(typeParent) {
    return typeParent(args[0]);
}
