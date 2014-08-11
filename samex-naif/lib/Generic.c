// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Class.h"
#include "type/Selector.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(genericBind) {
    // TODO: Should be bound to `classAddMethod` with proper arg order.
    zvalue selector = args[0];
    zvalue cls = args[1];
    zvalue function = args[2];

    classAddMethod(cls, selector, function);
    return NULL;
}

// Documented in spec.
FUN_IMPL_DECL(makeGeneric) {
    // TODO: Should be bound to `selectorFromName`.
    zvalue name = args[0];
    return selectorFromName(args[0]);
}
