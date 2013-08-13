/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * In-model Functions
 */

#include "impl.h"

#include <stdint.h>
#include <string.h>


/*
 * Private Definitions
 */

/**
 * Regular (non-generic) function structure.
 */
typedef struct {
    /** Minimum argument count. Always `>= 0`. */
    zint minArgs;

    /**
     * Maximum argument count. Always `>= minArgs`.
     */
    zint maxArgs;

    /** C function to call. */
    zfunction function;

    /** The function's name, if any. Used when producing stack traces. */
    zvalue name;
} FunctionInfo;

/**
 * Gets a pointer to the value's info.
 */
static FunctionInfo *getInfo(zvalue function) {
    return pbPayload(function);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue functionCall(zvalue function, zint argCount, const zvalue *args) {
    FunctionInfo *info = getInfo(function);

    if (argCount < info->minArgs) {
        die("Too few arguments for function call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for function call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    return info->function(argCount, args);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue functionFrom(zint minArgs, zint maxArgs, zfunction function,
        zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Function, sizeof(FunctionInfo));
    FunctionInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->function = function;
    info->name = name;

    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Function, call) {
    // The first argument is the function per se, and the rest are the
    // arguments to call it with.
    return functionCall(args[0], argCount - 1, &args[1]);
}

/* Documented in header. */
METH_IMPL(Function, canCall) {
    zvalue function = args[0];
    zvalue value = args[1];
    FunctionInfo *info = getInfo(function);

    return (info->maxArgs >= 1) ? value : NULL;
}

/* Documented in header. */
METH_IMPL(Function, debugString) {
    zvalue function = args[0];
    FunctionInfo *info = getInfo(function);

    zvalue result = stringFromUtf8(-1, "@(Function ");

    if (info->name != NULL) {
        result = stringAdd(result, funCall(GFN_debugString, 1, &info->name));
    } else {
        result = stringAdd(result, stringFromUtf8(-1, "(unknown)"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
METH_IMPL(Function, gcMark) {
    zvalue function = args[0];
    FunctionInfo *info = getInfo(function);

    pbMark(info->name);
    return NULL;
}

/* Documented in header. */
void pbBindFunction(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Function`.
    METH_BIND(Function, call);
    METH_BIND(Function, canCall);
    METH_BIND(Function, debugString);
    METH_BIND(Function, gcMark);
}

/* Documented in header. */
zvalue TYPE_Function = NULL;
