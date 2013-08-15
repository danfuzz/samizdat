/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * "Callable" Values (function-like things)
 */

#include "impl.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue funApply(zvalue function, zvalue args) {
    zint argCount = pbSize(args);
    zvalue argsArray[argCount];

    arrayFromList(argsArray, args);

    return funCall(function, argCount, argsArray);
}
