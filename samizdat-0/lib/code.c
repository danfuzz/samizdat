/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "lang.h"
#include "util.h"

#include <stddef.h>


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMutableBox) {
    zvalue value = (argCount == 1) ? args[0] : NULL;
    return makeMutableBox(value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(optValue) {
    zvalue function = args[0];
    zvalue value = FUN_CALL(function);

    return (value == NULL) ? EMPTY_LIST : listFromArray(1, &value);
}

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
PRIM_IMPL(makeYieldBox) {
    return makeYieldBox();
}
