// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#include <stdio.h>   // For `asprintf`.
#include <stdlib.h>  // For `free`.

#include "type/Builtin.h"
#include "type/Jump.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Returns `value` if it is a string. Returns `cm_toString(value)` if it is
 * a symbol; otherwise calls `debugString` on it.
 */
static zvalue ensureString(zvalue value) {
    if (classAccepts(CLS_String, value)) {
        return value;
    } else if (classAccepts(CLS_Symbol, value)) {
        return cm_toString(value);
    } else {
        return METH_CALL(debugString, value);
    }
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue value = state;
    zvalue name = METH_CALL(debugSymbol, value);

    if (name != NULL) {
        return utf8DupFromString(ensureString(name));
    }

    char *clsString = cm_debugString(get_class(value));
    char *result;

    asprintf(&result, "anonymous %s", clsString);
    free(clsString);

    return result;
}

/**
 * Inner implementation of `funCall`, which does *not* do argument validation,
 * nor debug and local frame setup/teardown.
 */
static zvalue funCall0(zvalue function, zint argCount, const zvalue *args) {
    zvalue funCls = get_class(function);

    // The first three cases are how we bottom out the recursion, instead of
    // calling `funCall0` on the `call` methods for `Builtin`, `Jump`, or
    // `Symbol`.
    if (funCls == CLS_Symbol) {
        return symbolCall(function, argCount, args);
    } else if (funCls == CLS_Builtin) {
        return builtinCall(function, argCount, args);
    } else if (funCls == CLS_Jump) {
        return jumpCall(function, argCount, args);
    } else {
        // The original `function` is some kind of higher layer function.
        // Use method dispatch to get to it: Prepend `function` as a new
        // first argument, and call the method `call` via its symbol.
        zvalue newArgs[argCount + 1];
        newArgs[0] = function;
        utilCpy(zvalue, &newArgs[1], args, argCount);
        return symbolCall(SYM(call), argCount + 1, newArgs);
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue funApply(zvalue function, zvalue args) {
    if (args == NULL) {
        return funCall(function, 0, NULL);
    } else {
        zint argCount = get_size(args);
        zvalue argsArray[argCount];
        arrayFromList(argsArray, args);
        return funCall(function, argCount, argsArray);
    }
}

// Documented in header.
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

// Documented in header.
zvalue mustNotYield(zvalue value) {
    die("Improper yield from `noYield` expression.");
}

//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(call) {
    MOD_USE(Builtin);
    MOD_USE(Jump);
    MOD_USE(Symbol);
}
