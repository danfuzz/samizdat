/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Callable.h"
#include "util.h"


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(nonlocalExit) {
    zvalue yieldFunction = args[0];
    zvalue value = (argCount == 2) ? args[1] : NULL;

    if (value == NULL) {
        FUN_CALL(yieldFunction);
    } else {
        FUN_CALL(yieldFunction, value);
    }

    die("Nonlocal exit function did not perform nonlocal exit.");
}
