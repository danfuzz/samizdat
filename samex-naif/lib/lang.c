/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "io.h"
#include "lang.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(Code_eval) {
    zvalue env = args[0];
    zvalue expressionNode = args[1];

    return langEval0(env, expressionNode);
}

/* Documented in spec. */
FUN_IMPL_DECL(Code_evalBinary) {
    zvalue env = args[0];
    zvalue path = args[1];

    ioCheckAbsolutePath(path);
    return datEvalBinary(env, path);
}

/* Documented in spec. */
FUN_IMPL_DECL(Lang0_convertToLang0) {
    zvalue node = args[0];

    // TODO: Probably do `withSimpleDefs` here.
    return node;
}

/* Documented in spec. */
FUN_IMPL_DECL(Lang0_languageOf) {
    return langLanguageOf0(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(Lang0_parseExpression) {
    return langParseExpression0(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(Lang0_parseProgram) {
    return langParseProgram0(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(Lang0_tokenize) {
    return langTokenize0(args[0]);
}
