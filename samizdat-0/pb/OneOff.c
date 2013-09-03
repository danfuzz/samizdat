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
 * Exported Definitions
 */

/* Documented in header. */
char *valToString(zvalue value) {
    if (value == NULL) {
        return strdup("(null)");
    }

    zvalue result = GFN_CALL(toString, value);
    zint size = utf8SizeFromString(result);
    char *str = utilAlloc(size + 1);

    utf8FromString(size + 1, str, result);
    return str;
}


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(OneOff) {
    MOD_USE(Value);

    GFN_keyOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "keyOf"));
    pbImmortalize(GFN_keyOf);

    GFN_nameOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "nameOf"));
    pbImmortalize(GFN_nameOf);

    GFN_toInt = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toInt"));
    pbImmortalize(GFN_toInt);

    GFN_toNumber = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toNumber"));
    pbImmortalize(GFN_toNumber);

    GFN_toString = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toString"));
    pbImmortalize(GFN_toString);

    GFN_valueOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "valueOf"));
    pbImmortalize(GFN_valueOf);
}

/* Documented in header. */
zvalue GFN_keyOf = NULL;

/* Documented in header. */
zvalue GFN_nameOf = NULL;

/* Documented in header. */
zvalue GFN_toInt = NULL;

/* Documented in header. */
zvalue GFN_toNumber = NULL;

/* Documented in header. */
zvalue GFN_toString = NULL;

/* Documented in header. */
zvalue GFN_valueOf = NULL;
