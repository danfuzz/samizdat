// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Class.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(classAddMethod) {
    classAddMethod(args[0], args[1], args[2]);
    return NULL;
}

// Documented in spec.
FUN_IMPL_DECL(classNameSelector) {
    return classNameSelector(args[0]);
}

// Documented in spec.
FUN_IMPL_DECL(classNameString) {
    return classNameString(args[0]);
}

// Documented in spec.
FUN_IMPL_DECL(classParent) {
    return classParent(args[0]);
}
