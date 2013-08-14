/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "lang.h"
#include "util.h"

#include <stddef.h>
#include <string.h>


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxFetch) {
    return boxFetch(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxCanStore) {
    zvalue box = args[0];

    return boxCanStore(box) ? box : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxStore) {
    zvalue result = (argCount == 2) ? args[1] : NULL;

    boxStore(args[0], result);
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mutableBox) {
    zvalue result = makeMutableBox();

    if (argCount == 1) {
        boxStore(result, args[0]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(nonlocalExit) {
    zvalue yieldFunction = args[0];
    zvalue value;

    if (argCount == 1) {
        value = NULL;
    } else {
        value = funCall(args[1], 0, NULL);
    }

    if (value == NULL) {
        funCall(yieldFunction, 0, NULL);
    } else {
        funCall(yieldFunction, 1, &value);
    }

    die("Nonlocal exit function did not perform nonlocal exit.");
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tokenize) {
    return langTokenize0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    return langTree0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEval0(ctx, expressionNode);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(yieldBox) {
    return makeYieldBox();
}
