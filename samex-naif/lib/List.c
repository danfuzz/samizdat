/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Function.h"
#include "type/Generator.h"
#include "type/List.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(makeList) {
    return listFromArray(argCount, args);
}
