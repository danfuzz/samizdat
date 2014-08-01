// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// In-model Generic Functions
//

#include <stdio.h>   // For `asprintf`.
#include <stdlib.h>  // For `free`.

#include "type/Builtin.h"
#include "type/Generic.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Generic function structure.
 */
typedef struct {
    /** Minimum argument count. Always `>= 1`. */
    zint minArgs;

    /** Maximum argument count. Always `>= minArgs`. */
    zint maxArgs;

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /**
     * The generic's name. Used when producing stack traces and to find the
     * corresponding selector.
     */
    zvalue name;

    #if USE_METHOD_TABLE
    /** The corresponding selector. */
    zvalue selector;
    #endif

    /** Bindings from class to function, keyed off of class id number. */
    zvalue functions[DAT_MAX_CLASSES];
} GenericInfo;

/**
 * Gets a pointer to the value's info.
 */
static GenericInfo *getInfo(zvalue generic) {
    return datPayload(generic);
}


/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue cls = state;
    char *clsString = valDebugString(cls);
    char *result;

    asprintf(&result, "class %s", clsString);
    free(clsString);

    return result;
}

/**
 * Finds the function binding for a given class, including walking up the
 * parent class chain. Returns `NULL` if there is no binding. On success, also
 * stores the bound class through `boundCls` if passed as non-`NULL`.
 */
static zvalue findByClass(zvalue generic, zvalue cls, zvalue *boundCls) {
    zvalue *functions = getInfo(generic)->functions;

    for (/*cls*/; cls != NULL; cls = classParent(cls)) {
        zvalue result = functions[classIndexUnchecked(cls)];
        if (result != NULL) {
            if (boundCls != NULL) {
                *boundCls = cls;
            }
            return result;
        }
    }

    return NULL;
}


//
// Module Definitions
//

/**
 * Actual implementation of generic function dispatch.
 */
zvalue genericCall(zvalue generic, zint argCount, const zvalue *args) {
    GenericInfo *info = getInfo(generic);

    if (argCount < info->minArgs) {
        die("Too few arguments for generic call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for generic call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    // Note: The replacement `firstCls` returned by `findByClass` is used
    // both for "same class" generics and for stack trace reporting.
    zvalue firstCls = get_class(args[0]);
    zvalue function = findByClass(generic, firstCls, &firstCls);

    #if USE_METHOD_TABLE
    // Verify that the same method is picked by the class's method table.
    zvalue classBinding = classFindMethod(args[0], info->selector);
    if (classBinding != function) {
        die("Eek! Method binding mismatch!");
    }
    #endif

    if (function == NULL) {
        die("No binding found: %s(%s, ...)",
            valDebugString(generic), valDebugString(args[0]));
    }

    UTIL_TRACE_START(callReporter, firstCls);
    zvalue result = funCall(function, argCount, args);
    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue genericFindByIndex(zvalue generic, zint index) {
    return getInfo(generic)->functions[index];
}


//
// Exported Definitions
//

// Documented in header.
void genericBind(zvalue generic, zvalue cls, zvalue function) {
    assertHasClass(generic, CLS_Generic);

    GenericInfo *info = getInfo(generic);
    zint index = classIndex(cls);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic: %s(%s, ...)",
            valDebugString(generic), valDebugString(cls));
    }

    info->functions[index] = function;

    #if USE_METHOD_TABLE
    classAddMethod(cls, info->selector, function);
    #endif
}

// Documented in header.
void genericBindPrim(zvalue generic, zvalue cls, zfunction function,
        const char *builtinName) {
    assertHasClass(generic, CLS_Generic);

    GenericInfo *info = getInfo(generic);
    zvalue name = (builtinName != NULL)
        ? stringFromUtf8(-1, builtinName)
        : info->name;
    zvalue builtin =
        makeBuiltin(info->minArgs, info->maxArgs, function, 0, name);

    genericBind(generic, cls, builtin);
}

// Documented in header.
void genericSeal(zvalue generic) {
    assertHasClass(generic, CLS_Generic);
    getInfo(generic)->sealed = true;
}

// Documented in header.
zvalue makeGeneric(zint minArgs, zint maxArgs, zvalue name) {
    assertHasClass(name, CLS_String);
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = datAllocValue(CLS_Generic, sizeof(GenericInfo));
    GenericInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->sealed = false;
    info->name = name;
    #if USE_METHOD_TABLE
    info->selector = selectorFromName(name);
    #endif

    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Generic, call) {
    // The first argument is the generic per se, and the rest are the
    // arguments to call it with.
    return genericCall(args[0], argCount - 1, &args[1]);
}

// Documented in header.
METH_IMPL(Generic, debugName) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);

    return info->name;
}

// Documented in header.
METH_IMPL(Generic, gcMark) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);

    datMark(info->name);
    for (zint i = 0; i < DAT_MAX_CLASSES; i++) {
        datMark(info->functions[i]);
    }

    return NULL;
}

/** Initializes the module. */
MOD_INIT(Generic) {
    MOD_USE(Function);
    MOD_USE(OneOff);

    // Note: The `objectModel` module initializes `CLS_Generic`.

    METH_BIND(Generic, call);
    METH_BIND(Generic, debugName);
    METH_BIND(Generic, gcMark);
}

// Documented in header.
zvalue CLS_Generic = NULL;
