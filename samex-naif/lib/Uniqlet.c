/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Uniqlet.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(makeUniqlet) {
    return makeUniqlet();
}

