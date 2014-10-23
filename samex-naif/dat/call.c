// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

#include <stdarg.h>
#include <stdio.h>   // For `asprintf`.

#include "type/Builtin.h"
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
 * Struct used to hold the salient info for generating a stack trace.
 */
typedef struct {
    /** Target being called. */
    zvalue target;

    /** Name of method being called. */
    zvalue name;
} StackTraceEntry;

/**
 * Returns a `dup()`ed string representing `value`. The result is the chars
 * of `value` if it is a string or symbol. Otherwise, it is the result of
 * calling `.debugString()` on `value`.
 */
static char *ensureString(zvalue value) {
    if (classAccepts(CLS_String, value)) {
        // No conversion.
    } else if (classAccepts(CLS_Symbol, value)) {
        value = cm_castFrom(CLS_String, value);
    } else {
        value = METH_CALL(value, debugString);
    }

    return utf8DupFromString(value);
}

/**
 * This is the function that handles emitting a context string for a method
 * call, when dumping the stack.
 */
static char *callReporter(void *state) {
    StackTraceEntry *ste = state;
    char *classStr =
        ensureString(METH_CALL(classOf(ste->target), debugSymbol));
    char *result;

    if (symbolEq(ste->name, SYM(call))) {
        // It's a function call (or function-like call).
        zvalue targetName = METH_CALL(ste->target, debugSymbol);

        if (targetName != NULL) {
            char *nameStr = ensureString(targetName);
            asprintf(&result, "%s (instance of %s)",
                nameStr, classStr);
            utilFree(nameStr);
        } else {
            asprintf(&result, "anonymous instance of %s", classStr);
        }
    } else {
        char *targetStr = cm_debugString(ste->target);
        char *nameStr = ensureString(ste->name);
        asprintf(&result, "%s.%s on %s", classStr, nameStr, targetStr);
        utilFree(targetStr);
        utilFree(nameStr);
    }

    utilFree(classStr);

    return result;
}

/**
 * Helper for `methCall`, which calls a function (which has just been looked
 * up in a method table). This does *not* do argument validation.
 */
static zvalue funCall(zvalue function, zint argCount, const zvalue *args) {
    zvalue funCls = classOf(function);

    // The first three cases are how we bottom out the recursion, instead of
    // calling `funCall` on the `call` methods for `Builtin` or `Symbol`.
    if (funCls == CLS_Builtin) {
        return builtinCall(function, argCount, args);
    } else {
        // The original `function` is some kind of higher layer function.
        // Use method dispatch to get to it.
        return methCall(function, SYM(call), argCount, args);
    }
}


//
// Exported Definitions
//

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
    if (argCount < 0) {
        die("Invalid argument count for method call: %lld", argCount);
    } else if ((args == NULL) && (argCount != 0)) {
        die("Method call argument inconsistency.");
    }

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

    StackTraceEntry ste = {.target = target, .name = name};
    UTIL_TRACE_START(callReporter, &ste);

    zstackPointer save = datFrameStart();
    zvalue result = funCall(function, argCount + 1, newArgs);
    datFrameReturn(save, result);

    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue mustNotYield(zvalue value) {
    die("Improper yield from `noYield` expression.");
}
