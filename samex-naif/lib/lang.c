// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "lang.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(Lang0_languageOf) {
    return langLanguageOf0(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Lang0_parseExpression) {
    return langParseExpression0(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Lang0_parseProgram) {
    return langParseProgram0(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Lang0_simplify) {
    return langSimplify0(args.elems[0], args.elems[1]);
}

// Documented in spec.
FUN_IMPL_DECL(Lang0_tokenize) {
    return langTokenize0(args.elems[0]);
}
