/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Module functions
 */

/** Documented in header. */
zvalue doNthLenient(znth function, zvalue value, zvalue n) {
    if (pbCoreTypeIs(n, DAT_Int)) {
        zint index = datZintFromInt(n);
        return (index < 0) ? NULL : function(value, index);
    } else {
        return NULL;
    }
}

/** Documented in header. */
zvalue doNthStrict(znth function, zvalue value, zvalue n) {
    if (pbCoreTypeIs(n, DAT_Int)) {
        zint index = datZintFromInt(n);
        if (index < 0) {
            die("Invalid index for nth (negative).");
        }
        return function(value, index);
    } else {
        die("Invalid type for nth (non-int).");
    }
}
