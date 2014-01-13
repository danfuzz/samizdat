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
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue collGet(zvalue coll, zvalue key) {
    return GFN_CALL(get, coll, key);
}

/* Documented in header. */
zvalue collPut(zvalue coll, zvalue key, zvalue value) {
    return GFN_CALL(put, coll, key, value);
}

/* Documented in header. */
zint collSize(zvalue coll) {
    return zintFromInt(GFN_CALL(sizeOf, coll));
}


/*
 * Type Definition: `Collection`
 */

/** Initializes the module. */
MOD_INIT(Collection) {
    MOD_USE(Value);

    GFN_cat = makeGeneric(1, -1, GFN_SAME_TYPE, stringFromUtf8(-1, "cat"));
    pbImmortalize(GFN_cat);

    GFN_del = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "del"));
    pbImmortalize(GFN_del);

    GFN_get = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "get"));
    pbImmortalize(GFN_get);

    GFN_keyList = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "keyList"));
    pbImmortalize(GFN_keyList);

    GFN_nthMapping =
        makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nthMapping"));
    pbImmortalize(GFN_nthMapping);

    GFN_put = makeGeneric(3, 3, GFN_NONE, stringFromUtf8(-1, "put"));
    pbImmortalize(GFN_put);

    GFN_sizeOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "sizeOf"));
    pbImmortalize(GFN_sizeOf);
}

/* Documented in header. */
zvalue GFN_cat = NULL;

/* Documented in header. */
zvalue GFN_del = NULL;

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_keyList = NULL;

/* Documented in header. */
zvalue GFN_nthMapping = NULL;

/* Documented in header. */
zvalue GFN_put = NULL;

/* Documented in header. */
zvalue GFN_sizeOf = NULL;
