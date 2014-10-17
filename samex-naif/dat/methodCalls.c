// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/methodCalls.h"
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
    char *result = utf8DupFromString(METH_CALL(debugString, x));
    inDebugString = false;

    return result;
}

// Documented in header.
zvalue cm_nth(zvalue x, zint index) {
    return METH_CALL(nth, x, intFromZint(index));
}

// Documented in header.
zvalue cm_new_SymbolTable0(zvalue first, ...) {
    if (first == NULL) {
        return EMPTY_SYMBOL_TABLE;
    }

    zint argCount = 1;
    va_list rest;

    va_start(rest, first);
    for (/*argCount*/; va_arg(rest, zvalue) != NULL; argCount++) /* empty */;
    va_end(rest);

    if ((argCount & 1) != 0) {
        die("Odd argument count for symbol table construction.");
    }

    zint size = argCount >> 1;
    zmapping elems[size];

    elems[0].key = first;

    va_start(rest, first);
    for (zint i = 1; i < argCount; i++) {
        zvalue one = va_arg(rest, zvalue);
        zmapping *elem = &elems[i >> 1];
        if ((i & 1) == 0) {
            elem->key = one;
        } else {
            elem->value = one;
        }
    }
    va_end(rest);

    return symbolTableFromArray(size, elems);
}

// Documented in header.
zorder cm_order(zvalue x, zvalue other) {
    // This frame usage avoids having the `zvalue` result of the call pollute
    // the stack.
    zstackPointer save = datFrameStart();
    zvalue result = valOrder(x, other);

    if (result == NULL) {
        die("Attempt to order unordered values.");
    }

    datFrameReturn(save, NULL);
    return zorderFromSymbol(result);
}

// Documented in header.
zint get_size(zvalue x) {
    return zintFromInt(METH_CALL(get_size, x));
}
