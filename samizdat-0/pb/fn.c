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
 * Helper Definitions
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

    /** Id to use for ordering comparisons. */
    zint orderId;
} FunctionInfo;

/**
 * Gets a pointer to the value's info.
 */
static FunctionInfo *fnInfo(zvalue function) {
    return pbPayload(function);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue doFnCall(zvalue function, zint argCount, const zvalue *args) {
    FunctionInfo *info = fnInfo(function);

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
zvalue fnFrom(zint minArgs, zint maxArgs, zfunction function, zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Function, sizeof(FunctionInfo));
    FunctionInfo *info = fnInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->function = function;
    info->name = name;
    info->orderId = pbOrderId();

    return result;
}

/* Documented in header. */
zfunction zfunctionFromFunction(zvalue function) {
    assertHasType(function, TYPE_Function);
    return fnInfo(function)->function;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Function, call) {
    // The first argument is the function per se, and the rest are the
    // arguments to call it with.
    return doFnCall(args[0], argCount - 1, &args[1]);
}

/* Documented in header. */
METH_IMPL(Function, canCall) {
    zvalue function = args[0];
    zvalue value = args[1];
    FunctionInfo *info = fnInfo(function);

    return (info->maxArgs >= 1) ? value : NULL;
}

/* Documented in header. */
METH_IMPL(Function, debugString) {
    zvalue function = args[0];
    FunctionInfo *info = fnInfo(function);

    zvalue result = stringFromUtf8(-1, "@(Function ");

    if (info->name != NULL) {
        result = stringAdd(result, fnCall(GFN_debugString, 1, &info->name));
    } else {
        result = stringAdd(result, stringFromUtf8(-1, "(unknown)"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
METH_IMPL(Function, gcMark) {
    zvalue function = args[0];
    FunctionInfo *info = fnInfo(function);

    pbMark(info->name);
    return NULL;
}

/* Documented in header. */
METH_IMPL(Function, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (fnInfo(v1)->orderId < fnInfo(v2)->orderId) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void pbBindFunction(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Function`.
    METH_BIND(Function, call);
    METH_BIND(Function, canCall);
    METH_BIND(Function, debugString);
    METH_BIND(Function, gcMark);
    METH_BIND(Function, order);
}

/* Documented in header. */
zvalue TYPE_Function = NULL;
