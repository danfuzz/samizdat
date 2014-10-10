// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#include <stdarg.h>
#include <stdio.h>   // For `asprintf`.

#include "type/Builtin.h"
#include "type/Jump.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Returns `value` if it is a string. Returns `String.castFrom(value)` if it
 * is a symbol; otherwise calls `debugString` on it.
 */
static zvalue ensureString(zvalue value) {
    if (classAccepts(CLS_String, value)) {
        return value;
    } else if (classAccepts(CLS_Symbol, value)) {
        return cm_castFrom(CLS_String, value);
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
    utilFree(clsString);

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
    zint argCount = (args == NULL) ? 0 : get_size(args);

    if (argCount == 0) {
        return funCall(function, 0, NULL);
    } else {
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

// Documented in header.
zvalue vaFunCall(zvalue function, ...) {
    zint size = 0;
    va_list rest;

    va_start(rest, function);
    for (;;) {
        if (va_arg(rest, zvalue) == NULL) {
            break;
        }
        size++;
    }
    va_end(rest);

    zvalue values[size];

    va_start(rest, function);
    for (zint i = 0; i < size; i++) {
        values[i] = va_arg(rest, zvalue);
    }
    va_end(rest);

    return funCall(function, size, values);
}

// Documented in header.
zvalue mustNotYield(zvalue value) {
    die("Improper yield from `noYield` expression.");
}
