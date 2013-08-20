/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Collection values
 */

#include "impl.h"
#include "type/Collection.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"


/*
 * Private Definitions
 */

/*
 * Exported Definitions
 */

/* Documented in header. */
zint valSize(zvalue coll) {
    return zintFromInt(GFN_CALL(size, coll));
}


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindCollection(void) {
    GFN_get = makeGeneric(2, 2, stringFromUtf8(-1, "get"));
    pbImmortalize(GFN_get);

    GFN_nth = makeGeneric(2, 2, stringFromUtf8(-1, "nth"));
    pbImmortalize(GFN_nth);

    GFN_size = makeGeneric(1, 1, stringFromUtf8(-1, "size"));
    pbImmortalize(GFN_size);
}

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_nth = NULL;

/* Documented in header. */
zvalue GFN_size = NULL;
