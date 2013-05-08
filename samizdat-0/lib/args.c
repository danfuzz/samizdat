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
void requireEven(zint argCount) {
    if ((argCount & 0x01) != 0) {
        die("Invalid non-even argument count for primitive: %lld", argCount);
    }
}

/** Documented in header. */
zvalue doNth(znth function, zint argCount, const zvalue *args) {
    requireRange(argCount, 2, 3);

    zvalue result = NULL;
    if (datTypeIs(args[1], DAT_INTEGER)) {
        result = function(args[0], datIntFromInteger(args[1]));
    }

    if (result == NULL) {
        return (argCount == 3) ? args[2] : NULL;
    } else {
        return result;
    }
}
