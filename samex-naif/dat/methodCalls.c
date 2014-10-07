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
char *cm_debugString(zvalue x) {
    if (x == NULL) {
        return utilStrdup("(null)");
    }

    if (SYM(debugString) == NULL) {
        die("Too early to call `debugString`.");
    } else if (inDebugString) {
        die("`cm_debugString` called recursively");
    }

    inDebugString = true;
    char *result = utf8DupFromString(METH_CALL(debugString, x));
    inDebugString = false;

    return result;
}

// Documented in header.
zvalue cm_fetch(zvalue x) {
    return METH_CALL(fetch, x);
}

// Documented in header.
zvalue cm_get(zvalue x, zvalue key) {
    return METH_CALL(get, x, key);
}

// Documented in header.
zvalue cm_nth(zvalue x, zint index) {
    return METH_CALL(nth, x, intFromZint(index));
}

// Documented in header.
zorder cm_order(zvalue x, zvalue other) {
    // This frame usage avoids having the `zvalue` result of the call pollute
    // the stack. See note on `valOrder` for more color.
    zstackPointer save = datFrameStart();
    zvalue result = valOrder(x, other);

    if (result == NULL) {
        die("Attempt to order unordered values.");
    }

    zorder order = zintFromInt(result);
    datFrameReturn(save, NULL);
    return order;
}

// Documented in header.
zvalue cm_store(zvalue x, zvalue value) {
    return (value == NULL)
        ? METH_CALL(store, x)
        : METH_CALL(store, x, value);
}

// Documented in header.
zvalue cm_put(zvalue x, zvalue key, zvalue value) {
    return METH_CALL(put, x, key, value);
}

// Documented in header.
zvalue cm_toString(zvalue x) {
    return METH_CALL(toString, x);
}

// Documented in header.
zvalue get_data(zvalue x) {
    return METH_CALL(get_data, x);
}

// Documented in header.
zvalue get_name(zvalue x) {
    return METH_CALL(get_name, x);
}

// Documented in header.
zint get_size(zvalue x) {
    return zintFromInt(METH_CALL(get_size, x));
}
