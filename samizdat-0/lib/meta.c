/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "lang.h"


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(samEval) {
    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEval0(ctx, expressionNode);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(samParseExpression) {
    return langParseExpression0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(samParseProgram) {
    return langParseProgram0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(samTokenize) {
    return langTokenize0(args[0]);
}
