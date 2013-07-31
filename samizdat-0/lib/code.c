/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "lang.h"
#include "util.h"

#include <stddef.h>
#include <string.h>


/*
 * Exported primitives
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxGet) {
    requireExactly(argCount, 1);

    return boxGet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxIsSet) {
    requireExactly(argCount, 1);

    zvalue box = args[0];

    return boxIsSet(box) ? box : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxSet) {
    requireRange(argCount, 1, 2);

    zvalue result = (argCount == 2) ? args[1] : NULL;

    boxSet(args[0], result);
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mutableBox) {
    requireRange(argCount, 0, 1);

    zvalue result = boxMutable();

    if (argCount == 1) {
        boxSet(result, args[0]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(nonlocalExit) {
    requireRange(argCount, 1, 2);

    zvalue yieldFunction = args[0];
    zvalue value;

    if (argCount == 1) {
        value = NULL;
    } else {
        value = datFnCall(args[1], 0, NULL);
    }

    if (value == NULL) {
        datFnCall(yieldFunction, 0, NULL);
    } else {
        datFnCall(yieldFunction, 1, &value);
    }

    die("Nonlocal exit function did not perform nonlocal exit.");
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tokenize) {
    requireExactly(argCount, 1);
    return langTokenize0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    requireExactly(argCount, 1);
    return langTree0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    requireExactly(argCount, 2);

    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEval0(ctx, expressionNode);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(yieldBox) {
    return boxYield();
}
