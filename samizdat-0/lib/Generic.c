/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Value.h"


/*
 * Private Definitions
 */

/**
 * Does most of the work of generic construction.
 */
static zvalue doMakeGeneric(zgenericFlags flags, zint argCount,
        const zvalue *args) {
    zvalue name = args[0];
    zvalue minArgsVal = args[1];
    zvalue maxArgsVal = (argCount == 3) ? args[1] : NULL;

    if (valEq(name, EMPTY_STRING)) {
        name = NULL;
    }

    zint minArgs = zintFromInt(minArgsVal);
    zint maxArgs = (maxArgsVal == NULL) ? -1 : zintFromInt(maxArgsVal);

    return makeGeneric(minArgs, maxArgs, flags, name);
}

/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(genericBind) {
    zvalue generic = args[0];
    zvalue type = args[1];
    zvalue callable = args[2];

    genericBind(generic, type, callable);
    return NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeRegularGeneric) {
    return doMakeGeneric(GFN_NONE, argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeUnitypeGeneric) {
    return doMakeGeneric(GFN_SAME_TYPE, argCount, args);
}
