/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(apply) {
    requireAtLeast(argCount, 1);

    zvalue function = args[0];

    switch (argCount) {
        case 1: {
            // Zero-argument call.
            return langCall(function, 0, NULL);
        }
        case 2: {
            // Just a "rest" listlet.
            return langApply(function, args[1]);
        }
    }

    // The hard case: We make a flattened array of all the initial arguments
    // followed by the contents of the "rest" listlet.

    zvalue rest = args[argCount - 1];
    zint restSize = datSize(rest);

    args++;
    argCount -= 2;

    zint flatSize = argCount + restSize;
    zvalue flatArgs[flatSize];

    for (zint i = 0; i < argCount; i++) {
        flatArgs[i] = args[i];
    }

    datArrayFromListlet(flatArgs + argCount, rest);
    return langCall(function, flatSize, flatArgs);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    requireExactly(argCount, 1);
    return langNodeFromProgramText(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    requireExactly(argCount, 2);

    zvalue contextMaplet = args[0];
    zvalue expressionNode = args[1];
    zcontext ctx = langCtxNew();

    langCtxBindAll(ctx, contextMaplet);
    return langEvalExpressionNode(ctx, expressionNode);
}
