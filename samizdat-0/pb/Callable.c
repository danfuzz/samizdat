/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Callable values (function-like things)
 */

#include "impl.h"
#include "type/Callable.h"
#include "type/Generic.h"
#include "type/String.h"
#include "type/Value.h"

#include <string.h>


/*
 * Private Definitions
 */

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    return pbDebugString((zvalue) state);
}

/**
 * Inner implementation of `funCall`, which does *not* do argument validation,
 * nor debug and local frame setup/teardown.
 */
static zvalue funCall0(zvalue function, zint argCount, const zvalue *args) {
    zint index = indexFromType(function->type);

    // The first two cases are how we bottom out the recursion, instead of
    // calling `funCall0` on the `call` methods for `Function` or `Generic`.
    switch (index) {
        case PB_INDEX_FUNCTION: {
            return functionCall(function, argCount, args);
        }
        case PB_INDEX_GENERIC: {
            return genericCall(function, argCount, args);
        }
        default: {
            // The original `function` is some kind of higher layer function.
            // Use generic dispatch to get to it: Prepend `function` as a new
            // first argument, and call the generic `call` via a recursive
            // call to `funCall0` to avoid the stack/frame setup.
            zvalue callImpl = genericFindByIndex(GFN_call, index);
            if (callImpl == NULL) {
                die("Attempt to call non-function.");
            } else {
                zvalue newArgs[argCount + 1];
                newArgs[0] = function;
                memcpy(&newArgs[1], args, argCount * sizeof(zvalue));
                return funCall0(callImpl, argCount + 1, newArgs);
            }
        }
    }
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue funCall(zvalue function, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    UTIL_TRACE_START(callReporter, function);
    zstackPointer save = pbFrameStart();

    zvalue result = funCall0(function, argCount, args);

    pbFrameReturn(save, result);
    UTIL_TRACE_END();

    return result;
}

/* Documented in header. */
extern zvalue funCallWith0(zvalue function);

/* Documented in header. */
extern zvalue funCallWith1(zvalue function, zvalue arg0);

/* Documented in header. */
extern zvalue funCallWith2(zvalue function, zvalue arg0, zvalue arg1);

/* Documented in header. */
extern zvalue funCallWith3(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2);

/* Documented in header. */
extern zvalue funCallWith4(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3);


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindCallable(void) {
    GFN_call = makeGeneric(1, -1, stringFromUtf8(-1, "call"));
    pbImmortalize(GFN_call);

    GFN_canCall = makeGeneric(2, 2, stringFromUtf8(-1, "canCall"));
    pbImmortalize(GFN_canCall);
}

/* Documented in header. */
zvalue GFN_call = NULL;

/* Documented in header. */
zvalue GFN_canCall = NULL;
