/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Int.h"
#include "type/Generic.h"
#include "type/OneOff.h"
#include "type/String.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue get(zvalue value, zvalue key) {
    return GFN_CALL(get, value, key);
}

/* Documented in header. */
zvalue get_name(zvalue value) {
    return GFN_CALL(get_name, value);
}

/* Documented in header. */
zvalue get_nameIfDefined(zvalue value) {
    if (GFN_CALL(canCall, GFN_get_name, value)) {
        return get_name(value);
    }

    return NULL;
}

/* Documented in header. */
zvalue nth(zvalue value, zint index) {
    return GFN_CALL(nth, value, intFromZint(index));
}

/* Documented in header. */
zint nthChar(zvalue value, zint index) {
    zvalue result = nth(value, index);
    return (result == NULL) ? -1 : zcharFromString(result);
}

/* Documented in header. */
zint sizeOf(zvalue value) {
    return zintFromInt(GFN_CALL(sizeOf, value));
}


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(OneOff) {
    MOD_USE(Value);

    GFN_cat = makeGeneric(1, -1, GFN_SAME_TYPE, stringFromUtf8(-1, "cat"));
    datImmortalize(GFN_cat);

    GFN_get = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "get"));
    datImmortalize(GFN_get);

    GFN_keyOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "keyOf"));
    datImmortalize(GFN_keyOf);

    GFN_get_name = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "get_name"));
    datImmortalize(GFN_get_name);

    GFN_nth = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nth"));
    datImmortalize(GFN_nth);

    GFN_sizeOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "sizeOf"));
    datImmortalize(GFN_sizeOf);

    GFN_toInt = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toInt"));
    datImmortalize(GFN_toInt);

    GFN_toNumber = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toNumber"));
    datImmortalize(GFN_toNumber);

    GFN_toString = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toString"));
    datImmortalize(GFN_toString);

    GFN_valueOf = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "valueOf"));
    datImmortalize(GFN_valueOf);
}

/* Documented in header. */
zvalue GFN_cat = NULL;

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_keyOf = NULL;

/* Documented in header. */
zvalue GFN_get_name = NULL;

/* Documented in header. */
zvalue GFN_nth = NULL;

/* Documented in header. */
zvalue GFN_sizeOf = NULL;

/* Documented in header. */
zvalue GFN_toInt = NULL;

/* Documented in header. */
zvalue GFN_toNumber = NULL;

/* Documented in header. */
zvalue GFN_toString = NULL;

/* Documented in header. */
zvalue GFN_valueOf = NULL;
