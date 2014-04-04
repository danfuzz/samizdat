/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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
 * Returns `value` if it is a string; otherwise calls `debugString` on it.
 */
static zvalue ensureString(zvalue value) {
    if (hasType(value, TYPE_String)) {
        return value;
    }

    return GFN_CALL(debugString, value);
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue value = state;
    zvalue name = nameOfIfDefined(value);

    if (name != NULL) {
        return utf8DupFromString(ensureString(name));
    }

    char *typeString = valDebugString(get_type(value));
    char *result;

    asprintf(&result, "(anonymous %s)", typeString);
    free(typeString);

    return result;
}

/**
 * Inner implementation of `funCall`, which does *not* do argument validation,
 * nor debug and local frame setup/teardown.
 */
static zvalue funCall0(zvalue function, zint argCount, const zvalue *args) {
    zint index = typeIndexOf(function);

    // The first three cases are how we bottom out the recursion, instead of
    // calling `funCall0` on the `call` methods for `Function`, `Generic`, or
    // `Jump`.
    switch (index) {
        case DAT_INDEX_BUILTIN: {
            return builtinCall(function, argCount, args);
        }
        case DAT_INDEX_GENERIC: {
            return genericCall(function, argCount, args);
        }
        case DAT_INDEX_JUMP: {
            return jumpCall(function, argCount, args);
        }
        default: {
            // The original `function` is some kind of higher layer function.
            // Use generic dispatch to get to it: Prepend `function` as a new
            // first argument, and call the generic `call` via a recursive
            // call to `funCall0` to avoid the stack/frame setup.
            zvalue callImpl = genericFindByIndex(GFN_call, index);
            if (callImpl == NULL) {
                die("Cannot call non-function: %s", valDebugString(function));
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
    zint argCount = sizeOf(args);
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
    zstackPointer save = datFrameStart();

    zvalue result = funCall0(function, argCount, args);

    datFrameReturn(save, result);
    UTIL_TRACE_END();

    return result;
}

/* Documented in header. */
zvalue funJump(zvalue function, zvalue optArg) {
    if (optArg == NULL) {
        funCall(function, 0, NULL);
    } else {
        funCall(function, 1, &optArg);
    }

    die("Improper return from jump.");
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
extern zvalue funCallWith7(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6);
extern zvalue funCallWith8(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7);
extern zvalue funCallWith9(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8);
extern zvalue funCallWith10(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9);
extern zvalue funCallWith11(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10);
extern zvalue funCallWith12(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11);
extern zvalue funCallWith13(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12);
extern zvalue funCallWith14(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13);
extern zvalue funCallWith15(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13, zvalue arg14);
extern zvalue funCallWith16(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15);
extern zvalue funCallWith17(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
    zvalue arg16);
extern zvalue funCallWith18(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
    zvalue arg16, zvalue arg17);
extern zvalue funCallWith19(zvalue function, zvalue arg0, zvalue arg1,
    zvalue arg2, zvalue arg3, zvalue arg4, zvalue arg5, zvalue arg6,
    zvalue arg7, zvalue arg8, zvalue arg9, zvalue arg10, zvalue arg11,
    zvalue arg12, zvalue arg13, zvalue arg14, zvalue arg15,
    zvalue arg16, zvalue arg17, zvalue arg18);


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(Function) {
    MOD_USE(Value);

    GFN_call = makeGeneric(1, -1, GFN_NONE, stringFromUtf8(-1, "call"));
    datImmortalize(GFN_call);

    GFN_canCall = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "canCall"));
    datImmortalize(GFN_canCall);
}

/* Documented in header. */
zvalue GFN_call = NULL;

/* Documented in header. */
zvalue GFN_canCall = NULL;
