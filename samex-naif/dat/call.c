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
 * This is the function that handles emitting a context string for a function
 * call, when dumping the stack.
 */
static char *funcReporter(void *state) {
    zvalue value = state;
    zvalue name = METH_CALL(debugSymbol, value);

    if (name != NULL) {
        return utf8DupFromString(ensureString(name));
    }

    char *clsString = cm_debugString(classOf(value));
    char *result;

    asprintf(&result, "anonymous %s", clsString);
    utilFree(clsString);

    return result;
}

/**
 * This is the function that handles emitting a context string for a method
 * call, when dumping the stack.
 */
static char *methReporter(void *state) {
    zvalue cls = state;
    char *clsString = cm_debugString(cls);
    char *result;

    asprintf(&result, "class %s", clsString);
    utilFree(clsString);

    return result;
}

/**
 * Inner implementation of `funCall`, which does *not* do argument validation,
 * nor debug and local frame setup/teardown.
 */
static zvalue funCall0(zvalue function, zint argCount, const zvalue *args) {
    zvalue funCls = classOf(function);

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
        // Use method dispatch to get to it.
        return methCall(function, SYM(call), argCount, args);
    }
}


//
// Module Definitions
//

// Documented in header.
zvalue symbolCall(zvalue symbol, zint argCount, const zvalue *args) {
    if (argCount < 1) {
        die("Too few arguments for symbol call.");
    }

    return methCall(args[0], symbol, argCount - 1, &args[1]);
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

    UTIL_TRACE_START(funcReporter, function);
    zstackPointer save = datFrameStart();
    zvalue result = funCall0(function, argCount, args);
    datFrameReturn(save, result);
    UTIL_TRACE_END();

    return result;
}

// Documented in header.
zvalue methApply(zvalue target, zvalue name, zvalue args) {
    zint argCount = (args == NULL) ? 0 : get_size(args);

    if (argCount == 0) {
        return methCall(target, name, 0, NULL);
    } else {
        zvalue argsArray[argCount];
        arrayFromList(argsArray, args);
        return methCall(target, name, argCount, argsArray);
    }
}

// Documented in header.
zvalue methCall(zvalue target, zvalue name, zint argCount,
        const zvalue *args) {
    zint index = symbolIndex(name);
    zvalue cls = classOf(target);
    zvalue function = classFindMethodUnchecked(cls, index);

    if (function == NULL) {
        zvalue nameStr = cm_castFrom(CLS_String, name);
        die("Unbound method: %s.%s", cm_debugString(cls),
            cm_debugString(nameStr));
    }

    // Prepend `target` as a new first argument for a call to `function`.
    zvalue newArgs[argCount + 1];
    newArgs[0] = target;
    utilCpy(zvalue, &newArgs[1], args, argCount);

    UTIL_TRACE_START(methReporter, cls);
    zstackPointer save = datFrameStart();
    zvalue result = funCall0(function, argCount + 1, newArgs);
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
