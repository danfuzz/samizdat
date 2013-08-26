/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator protocol
 */

#include "impl.h"
#include "type/Box.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"


/*
 * Type Definition: `Generator`
 *
 * This includes bindings for the methods on all the core types.
 */

/* Documented in header. */
METH_IMPL(Collection, next) {
    zvalue coll = args[0];
    zvalue box = args[1];

    zvalue value = collNth(coll, 0);

    if (value == NULL) {
        GFN_CALL(store, box);
        return NULL;
    } else {
        GFN_CALL(store, box, value);
        return GFN_CALL(slice, coll, PB_1);
    }
}

/* Documented in header. */
METH_IMPL(List, collect) {
    zvalue list = args[0];

    // Easy case: A list is its own collected result.
    return list;
}

/* Documented in header. */
METH_IMPL(Map, collect) {
    zvalue map = args[0];

    zint size = collSize(map);
    zvalue arr[size];

    for (zint i = 0; i < size; i++) {
        arr[i] = collNth(map, i);
    }

    return listFromArray(size, arr);
}

/* Documented in header. */
METH_IMPL(String, collect) {
    zvalue string = args[0];

    zint size = collSize(string);
    zchar chars[size];
    zvalue arr[size];

    zcharsFromString(chars, string);

    for (zint i = 0; i < size; i++) {
        arr[i] = stringFromZchar(chars[i]);
    }

    return listFromArray(size, arr);
}

/* Documented in header. */
void datBindGenerator(void) {
    GFN_collect = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "collect"));
    pbImmortalize(GFN_collect);

    GFN_next = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "next"));
    pbImmortalize(GFN_next);

    METH_BIND(List,   collect);
    METH_BIND(Map,    collect);
    METH_BIND(String, collect);
    genericBindCore(GFN_next, TYPE_List,   Collection_next);
    genericBindCore(GFN_next, TYPE_Map,    Collection_next);
    genericBindCore(GFN_next, TYPE_String, Collection_next);
}

/* Documented in header. */
zvalue GFN_collect = NULL;

/* Documented in header. */
zvalue GFN_next = NULL;
