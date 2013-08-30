/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/OneOff.h"
#include "type/String.h"


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindOneOff(void) {
    GFN_nameOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "nameOf"));
    pbImmortalize(GFN_nameOf);

    GFN_toInt = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toInt"));
    pbImmortalize(GFN_toInt);

    GFN_toNumber = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toNumber"));
    pbImmortalize(GFN_toNumber);

    GFN_toString = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toString"));
    pbImmortalize(GFN_toString);
}

/* Documented in header. */
zvalue GFN_nameOf = NULL;

/* Documented in header. */
zvalue GFN_toInt = NULL;

/* Documented in header. */
zvalue GFN_toNumber = NULL;

/* Documented in header. */
zvalue GFN_toString = NULL;
