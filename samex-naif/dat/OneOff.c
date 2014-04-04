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
zint get_size(zvalue value) {
    return zintFromInt(GFN_CALL(get_size, value));
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

    GFN_get_key = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "get_key"));
    datImmortalize(GFN_get_key);

    GFN_get_name = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "get_name"));
    datImmortalize(GFN_get_name);

    GFN_get_size = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "get_size"));
    datImmortalize(GFN_get_size);

    GFN_get_value = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "get_value"));
    datImmortalize(GFN_get_value);

    GFN_nth = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nth"));
    datImmortalize(GFN_nth);

    GFN_toInt = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toInt"));
    datImmortalize(GFN_toInt);

    GFN_toNumber = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toNumber"));
    datImmortalize(GFN_toNumber);

    GFN_toString = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "toString"));
    datImmortalize(GFN_toString);
}

/* Documented in header. */
zvalue GFN_cat = NULL;

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_get_key = NULL;

/* Documented in header. */
zvalue GFN_get_name = NULL;

/* Documented in header. */
zvalue GFN_get_size = NULL;

/* Documented in header. */
zvalue GFN_get_value = NULL;

/* Documented in header. */
zvalue GFN_nth = NULL;

/* Documented in header. */
zvalue GFN_toInt = NULL;

/* Documented in header. */
zvalue GFN_toNumber = NULL;

/* Documented in header. */
zvalue GFN_toString = NULL;
