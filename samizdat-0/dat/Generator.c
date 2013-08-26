/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator protocol
 */

#include "impl.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/String.h"
#include "type/Type.h"


/*
 * Type Definition: `Generator`
 */

/* Documented in header. */
void datBindGenerator(void) {
    GFN_collect = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "collect"));
    pbImmortalize(GFN_collect);

    GFN_next = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "next"));
    pbImmortalize(GFN_next);
}

/* Documented in header. */
zvalue GFN_collect = NULL;

/* Documented in header. */
zvalue GFN_next = NULL;
