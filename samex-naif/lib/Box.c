// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stddef.h>

#include "type/Box.h"

#include "impl.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(Box_makeCell) {
    zvalue value = (argCount == 1) ? args[0] : NULL;
    return makeCell(value);
}

/* Documented in spec. */
FUN_IMPL_DECL(Box_makePromise) {
    return makePromise();
}

/* Documented in spec. */
FUN_IMPL_DECL(Box_makeResult) {
    zvalue value = (argCount == 1) ? args[0] : NULL;
    return makeResult(value);
}
