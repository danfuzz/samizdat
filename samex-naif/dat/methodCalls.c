// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/methodCalls.h"
#include "type/Int.h"
#include "type/String.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Flag indicating that `cm_debugString` is in progress, as it's bad news
 * if the function is called recursively.
 */
static bool inDebugString = false;


//
// Exported Definitions
//

// Documented in header.
zvalue cm_store(zvalue box, zvalue value) {
    return (value == NULL)
        ? METH_CALL(store, box)
        : METH_CALL(store, box, value);
}

// Documented in header.
zvalue get_data(zvalue value) {
    return METH_CALL(get_data, value);
}

// Documented in header.
zvalue cm_get(zvalue value, zvalue key) {
    return METH_CALL(get, value, key);
}

// Documented in header.
zvalue get_name(zvalue value) {
    return METH_CALL(get_name, value);
}

// Documented in header.
zint get_size(zvalue value) {
    return zintFromInt(METH_CALL(get_size, value));
}

// Documented in header.
zvalue cm_nth(zvalue value, zint index) {
    return METH_CALL(nth, value, intFromZint(index));
}

// Documented in header.
char *cm_debugString(zvalue value) {
    if (value == NULL) {
        return utilStrdup("(null)");
    }

    if (SYM(debugString) == NULL) {
        die("Too early to call `debugString`.");
    } else if (inDebugString) {
        die("`cm_debugString` called recursively");
    }

    inDebugString = true;
    char *result = utf8DupFromString(METH_CALL(debugString, value));
    inDebugString = false;

    return result;
}

// Documented in header.
zvalue cm_toString(zvalue value) {
    return METH_CALL(toString, value);
}

// Documented in header.
zorder cm_order(zvalue value, zvalue other) {
    // This frame usage avoids having the `zvalue` result of the call pollute
    // the stack. See note on `valOrder` for more color.
    zstackPointer save = datFrameStart();
    zvalue result = valOrder(value, other);

    if (result == NULL) {
        die("Attempt to order unordered values.");
    }

    zorder order = zintFromInt(result);
    datFrameReturn(save, NULL);
    return order;
}
