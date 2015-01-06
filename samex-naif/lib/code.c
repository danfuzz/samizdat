// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "io.h"
#include "lang.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(Code_eval) {
    zvalue env = args.elems[0];
    zvalue expressionNode = args.elems[1];

    return langEval0(env, expressionNode);
}

// Documented in spec.
FUN_IMPL_DECL(Code_evalBinary) {
    zvalue env = args.elems[0];
    zvalue path = args.elems[1];

    ioCheckAbsolutePath(path);
    return datEvalBinary(env, path);
}
