/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Function.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Concatenates all the arguments into a unified string, returning that
 * string. It must be freed with `utilFree()` when done.
 */
static char *unifiedString(zint argCount, const zvalue *args,
        const char *ifNone) {
    if (argCount == 0) {
        return utilStrdup((ifNone == NULL) ? "" : ifNone);
    }

    zint size = 1;  // Starts at 1, to count the terminal null byte.
    for (zint i = 0; i < argCount; i++) {
        size += utf8SizeFromString(args[i]);
    }

    char *result = utilAlloc(size);
    for (zint i = 0, at = 0; i < argCount; i++) {
        at += utf8FromString(size - at, &result[at], args[i]);
    }

    return result;
}


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(die) {
    char *str = unifiedString(argCount, args, "Alas.");
    die("%s", str);
}

/* Documented in spec. */
FUN_IMPL_DECL(note) {
    char *str = unifiedString(argCount, args, NULL);

    note("%s", str);
    utilFree(str);

    return NULL;
}
