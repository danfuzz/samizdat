// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Value.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(eq) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valEq(value, other);
}

// Documented in spec.
FUN_IMPL_DECL(get_class) {
    return get_class(args[0]);
}

// Documented in spec.
FUN_IMPL_DECL(order) {
    zvalue value = args[0];
    zvalue other = args[1];

    return valOrder(value, other);
}
