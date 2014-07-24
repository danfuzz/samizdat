// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// In-model Generic Functions
//

#include <stdio.h>   // For `asprintf`.
#include <stdlib.h>  // For `free`.

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Generic.h"
#include "type/String.h"
#include "type/Value.h"
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

    /** Flags (restrictions on arguments). */
    zgenericFlags flags;

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Bindings from type to function, keyed off of type sequence number. */
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
    zvalue type = state;
    char *typeString = valDebugString(type);
    char *result;

    asprintf(&result, "type %s", typeString);
    free(typeString);

    return result;
}

/**
 * Finds the function binding for a given type, including walking up the
 * parent type chain. Returns `NULL` if there is no binding. On success, also
 * stores the bound type through `boundType` if passed as non-`NULL`.
 */
static zvalue findByType(zvalue generic, zvalue type, zvalue *boundType) {
    zvalue *functions = getInfo(generic)->functions;

    for (/*type*/; type != NULL; type = classParent(type)) {
        zvalue result = functions[classIndexUnchecked(type)];
        if (result != NULL) {
            if (boundType != NULL) {
                *boundType = type;
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

    // Note: The replacement `firstType` returned by `findByType` is used
    // both for "same type" generics and for stack trace reporting.
    zvalue firstType = get_class(args[0]);
    zvalue function = findByType(generic, firstType, &firstType);

    if (function == NULL) {
        die("No binding found: %s(%s, ...)",
            valDebugString(generic), valDebugString(args[0]));
    }

    if (info->flags & GFN_SAME_TYPE) {
        for (zint i = 1; i < argCount; i++) {
            if (!hasClass(args[i], firstType)) {
                die("Type mismatch on argument #%lld of: %s(%s, ...)",
                    i, valDebugString(generic), valDebugString(args[0]));
            }
        }
    }

    UTIL_TRACE_START(callReporter, firstType);
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
void genericBind(zvalue generic, zvalue type, zvalue function) {
    assertHasClass(generic, TYPE_Generic);

    GenericInfo *info = getInfo(generic);
    zint index = classIndex(type);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic: %s(%s, ...)",
            valDebugString(generic), valDebugString(type));
    }

    info->functions[index] = function;
}

// Documented in header.
void genericBindPrim(zvalue generic, zvalue type, zfunction function,
        const char *builtinName) {
    assertHasClass(generic, TYPE_Generic);

    GenericInfo *info = getInfo(generic);
    zvalue name = (builtinName != NULL)
        ? stringFromUtf8(-1, builtinName)
        : info->name;
    zvalue builtin =
        makeBuiltin(info->minArgs, info->maxArgs, function, 0, name);

    genericBind(generic, type, builtin);
}

// Documented in header.
void genericSeal(zvalue generic) {
    assertHasClass(generic, TYPE_Generic);
    getInfo(generic)->sealed = true;
}

// Documented in header.
zvalue makeGeneric(zint minArgs, zint maxArgs, zgenericFlags flags,
        zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = datAllocValue(TYPE_Generic, sizeof(GenericInfo));
    GenericInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->flags = flags;
    info->sealed = false;
    info->name = name;

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

    // Note: The `typeSystem` module initializes `TYPE_Generic`.

    METH_BIND(Generic, call);
    METH_BIND(Generic, debugName);
    METH_BIND(Generic, gcMark);
}

// Documented in header.
zvalue TYPE_Generic = NULL;
