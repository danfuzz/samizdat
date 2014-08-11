// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Class.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/Selector.h"
#include "type/String.h"
#include "type/Value.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(genericBind) {
    #if DAT_USE_METHOD_TABLE

    zvalue selector = args[0];
    zvalue cls = args[1];
    zvalue function = args[2];

    classAddMethod(cls, selector, function);
    return NULL;

    #else
    zvalue generic = args[0];
    zvalue cls = args[1];
    zvalue function = args[2];

    genericBind(generic, cls, function);
    return NULL;
    #endif
}

// Documented in spec.
FUN_IMPL_DECL(makeGeneric) {
    #if DAT_USE_METHOD_TABLE

    zvalue name = args[0];
    return selectorFromName(args[0]);

    #else
    zvalue name = args[0];
    zvalue minArgsVal = args[1];
    zvalue maxArgsVal = (argCount == 3) ? args[2] : NULL;

    if (valEq(name, EMPTY_STRING)) {
        name = NULL;
    }

    zint minArgs = zintFromInt(minArgsVal);
    zint maxArgs = (maxArgsVal == NULL) ? -1 : zintFromInt(maxArgsVal);

    return makeGeneric(minArgs, maxArgs, name);
    #endif
}
