/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/String.h"
#include "util.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(die) {
    if (argCount == 1) {
        zvalue message = args[0];
        zint size = utf8SizeFromString(message);
        char str[size + 1];
        utf8FromString(size + 1, str, message);
        die("%s", str);
    }

    die("Alas.");
}

/* Documented in spec. */
FUN_IMPL_DECL(note) {
    zvalue message = args[0];
    zint size = utf8SizeFromString(message);
    char str[size + 1];
    utf8FromString(size + 1, str, message);

    note("%s", str);
    return NULL;
}
