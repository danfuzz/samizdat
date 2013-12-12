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
PRIM_IMPL(Lang0_eval) {
    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEval0(ctx, expressionNode);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(Lang0_evalBinary) {
    return pbEvalBinary(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(Lang0_parseExpression) {
    return langParseExpression0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(Lang0_parseProgram) {
    return langParseProgram0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(Lang0_tokenize) {
    return langTokenize0(args[0]);
}
