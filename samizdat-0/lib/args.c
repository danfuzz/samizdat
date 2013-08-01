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
void requireExactly(zint argCount, zint required) {
    if (argCount != required) {
        die("Invalid argument count for primitive: %lld != %lld",
            argCount, required);
    }
}

/** Documented in header. */
void requireRange(zint argCount, zint min, zint max) {
    if (argCount < min) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, min);
    } else if (argCount > max) {
        die("Invalid argument count for primitive: %lld > %lld",
            argCount, max);
    }
}

/** Documented in header. */
void requireAtLeast(zint argCount, zint minimum) {
    if (argCount < minimum) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, minimum);
    }
}

/** Documented in header. */
zvalue doNthLenient(znth function, zvalue value, zvalue n) {
    if (datCoreTypeIs(n, DAT_Int)) {
        zint index = datZintFromInt(n);
        return (index < 0) ? NULL : function(value, index);
    } else {
        return NULL;
    }
}

/** Documented in header. */
zvalue doNthStrict(znth function, zvalue value, zvalue n) {
    if (datCoreTypeIs(n, DAT_Int)) {
        zint index = datZintFromInt(n);
        if (index < 0) {
            die("Invalid index for nth (negative).");
        }
        return function(value, index);
    } else {
        die("Invalid type for nth (non-int).");
    }
}
