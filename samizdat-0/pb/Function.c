/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Function values
 */

#include "impl.h"
#include "type/Function.h"
#include "type/Generic.h"
#include "type/List.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"

#include <stdio.h>  // For `asprintf`.
#include <stdlib.h> // For `free`.


/*
 * Private Definitions
 */

/**
 * Gets the name of the given value if it has a name. Otherwise returns
 * the value.
 */
static zvalue nameOrValue(zvalue value) {
    if (GFN_CALL(canCall, GFN_nameOf, value)) {
        zvalue name = GFN_CALL(nameOf, value);
        if (name != NULL) {
            return name;
        }
    }

    return value;
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue value = state;

    value = nameOrValue(value);

    if (GFN_CALL(canCall, GFN_toString, value)) {
        return valToString(value);
    }

    zvalue type = nameOrValue(typeOf(value));
    char *typeString;
    char *result;

    if (GFN_CALL(canCall, GFN_toString, value)) {
        typeString = valToString(type);
    } else {
        typeString = valDebugString(type);
    }

    asprintf(&result, "(anonymous %s)", typeString);
    free(typeString);

    return result;
}

/**
 * Inner implementation of `funCall`, which does *not* do argument validation,
 * nor debug and local frame setup/teardown.
 */
static zvalue funCall0(zvalue function, zint argCount, const zvalue *args) {
    zint index = indexFromTrueType(function->type);

    // The first two cases are how we bottom out the recursion, instead of
    // calling `funCall0` on the `call` methods for `Function` or `Generic`.
    switch (index) {
        case PB_INDEX_BUILTIN: {
            return builtinCall(function, argCount, args);
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
                utilCpy(zvalue, &newArgs[1], args, argCount);
                return funCall0(callImpl, argCount + 1, newArgs);
            }
        }
    }
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue funApply(zvalue function, zvalue args) {
    zint argCount = collSize(args);
    zvalue argsArray[argCount];

    arrayFromList(argsArray, args);

    return funCall(function, argCount, argsArray);
}

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

// All documented in header.
extern zvalue funCallWith0(zvalue function);
extern zvalue funCallWith1(zvalue function, zvalue arg0);
extern zvalue funCallWith2(zvalue function, zvalue arg0, zvalue arg1);
extern zvalue funCallWith3(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2);
extern zvalue funCallWith4(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3);
extern zvalue funCallWith5(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4);
extern zvalue funCallWith6(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5);


/*
 * Type Definition
 */

/* Documented in header. */
void pbBindFunction(void) {
    GFN_call = makeGeneric(1, -1, GFN_NONE, stringFromUtf8(-1, "call"));
    pbImmortalize(GFN_call);

    GFN_canCall = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "canCall"));
    pbImmortalize(GFN_canCall);
}

/* Documented in header. */
zvalue GFN_call = NULL;

/* Documented in header. */
zvalue GFN_canCall = NULL;
