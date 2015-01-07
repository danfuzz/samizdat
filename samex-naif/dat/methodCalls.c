// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/methodCalls.h"
#include "type/Cmp.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"

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
    char *result = utf8DupFromString(METH_CALL(x, debugString));
    inDebugString = false;

    return result;
}

// Documented in header.
zvalue cm_nth(zvalue x, zint index) {
    return METH_CALL(x, nth, intFromZint(index));
}

// Documented in header.
zvalue cm_newBox0(zvalue cls, zvalue value) {
    return (value == NULL)
        ? METH_CALL(cls, new)
        : METH_CALL(cls, new, value);
}

// Documented in header.
zorder cm_order(zvalue x, zvalue other) {
    // This frame usage avoids having the `zvalue` result of the call pollute
    // the stack.
    zstackPointer save = datFrameStart();
    zvalue result = cmpOrder(x, other);

    if (result == NULL) {
        die("Attempt to order unordered values.");
    }

    zorder order = zorderFromSymbol(result);
    datFrameReturn(save, NULL);

    return order;
}

// Documented in header.
zvalue cm_store(zvalue x, zvalue value) {
    return (value == NULL)
        ? METH_CALL(x, store)
        : METH_CALL(x, store, value);
}

// Documented in header.
zint get_size(zvalue x) {
    return zintFromInt(METH_CALL(x, get_size));
}
