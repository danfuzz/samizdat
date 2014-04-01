/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Type.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(typeFromName) {
    return typeFromName(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(typeIsDerivedData) {
    zvalue type = args[0];
    return typeIsDerivedData(type) ? type : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(typeOf) {
    return typeOf(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(typeParent) {
    return typeParent(args[0]);
}
