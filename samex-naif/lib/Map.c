/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Value.h"
#include "util.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(makeValueMap) {
    zint size = argCount - 1;
    zvalue value = args[size];

    if (size == 0) {
        return EMPTY_MAP;
    }

    zmapping mappings[size];

    for (zint i = 0; i < size; i++) {
        mappings[i].key = args[i];
        mappings[i].value = value;
    }

    return mapFromArray(size, mappings);
}
