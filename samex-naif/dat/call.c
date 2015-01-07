// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Function / method calling
//

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
    if (typeAccepts(CLS_String, value)) {
        // No conversion.
    } else if (typeAccepts(CLS_Symbol, value)) {
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
            result = utilFormat("%s (instance of %s)", nameStr, classStr);
            utilFree(nameStr);
        } else {
            result = utilFormat("anonymous instance of %s", classStr);
        }
    } else {
        char *targetStr = cm_debugString(ste->target);
        char *nameStr = ensureString(ste->name);
        result = utilFormat("%s.%s on %s", classStr, nameStr, targetStr);
        utilFree(targetStr);
        utilFree(nameStr);
    }

    utilFree(classStr);

    return result;
}

/**
 * Helper for `methCall`, which does most of the work but skips argument
 * validation, reference frame, and stack trace setup.
 */
static zvalue methCall0(zvalue target, zint nameIndex, zarray args) {
    zvalue cls = classOf(target);

    if ((cls == CLS_Builtin) && (nameIndex == SYMIDX(call))) {
        // We are doing an invocation of `Builtin.call()`. Handle this as a
        // special case, in order to break the recursion.
        return builtinCall(target, args);
    }

    zvalue function = classFindMethodUnchecked(cls, nameIndex);

    if (function == NULL) {
        zvalue nameStr = cm_castFrom(CLS_String, symbolFromIndex(nameIndex));
        die("Unbound method: %s.%s", cm_debugString(cls),
            cm_debugString(nameStr));
    }

    // Prepend `target` as a new first argument for a call to `function`.
    zint newSize = args.size + 1;
    zvalue newArgs[newSize];
    newArgs[0] = target;
    utilCpy(zvalue, &newArgs[1], args.elems, args.size);

    // Invoke `function.call(target, args*)`.
    return methCall0(function, SYMIDX(call), (zarray) {newSize, newArgs});
}


//
// Exported Definitions
//

// Documented in header.
zvalue methApply(zvalue target, zvalue name, zvalue args) {
    return (args == NULL)
        ? methCall(target, name, EMPTY_ZARRAY)
        : methCall(target, name, zarrayFromList(args));
}

// Documented in header.
zvalue methCall(zvalue target, zvalue name, zarray args) {
    zint nameIndex = symbolIndex(name);

    StackTraceEntry ste = {.target = target, .name = name};
    UTIL_TRACE_START(callReporter, &ste);

    zstackPointer save = datFrameStart();
    zvalue result = methCall0(target, nameIndex, args);
    datFrameReturn(save, result);

    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue mustNotYield(zvalue value) {
    die("Improper yield from `noYield` expression.");
}
