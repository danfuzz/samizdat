// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/String.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Concatenates all the arguments into a unified string, returning that
 * string. It must be freed with `utilFree()` when done.
 */
static char *unifiedString(zarray args, const char *ifNone) {
    if (args.size == 0) {
        return utilStrdup((ifNone == NULL) ? "" : ifNone);
    }

    zint size = 1;  // Starts at 1, to count the terminal null byte.
    for (zint i = 0; i < args.size; i++) {
        size += utf8SizeFromString(args.elems[i]);
    }

    char *result = utilAlloc(size);
    for (zint i = 0, at = 0; i < args.size; i++) {
        at += utf8FromString(size - at, &result[at], args.elems[i]);
        at--;  // Back up over the terminal null byte.
    }

    return result;
}


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(die) {
    char *str = unifiedString(args, "Alas.");
    die("%s", str);
}

// Documented in spec.
FUN_IMPL_DECL(note) {
    char *str = unifiedString(args, NULL);

    note("%s", str);
    utilFree(str);

    return NULL;
}
