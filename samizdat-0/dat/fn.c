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
 * Helper definitions
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

    /** Closure state. */
    zvalue state;

    /** The function's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Uniqlet to use for ordering comparisons. */
    zvalue orderId;
} DatFunction;

/**
 * Gets a pointer to the value's info.
 */
static DatFunction *fnInfo(zvalue function) {
    return datPayload(function);
}

/**
 * Gets the order id, initializing it if necessary.
 */
static zvalue fnOrderId(zvalue function) {
    DatFunction *info = fnInfo(function);
    zvalue orderId = info->orderId;

    if (orderId == NULL) {
        orderId = info->orderId = datUniqlet();
    }

    return orderId;
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue name = fnInfo((zvalue) state)->name;

    if (name != NULL) {
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
        return strdup(nameStr);
    } else {
        return "(unknown)";
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datApply(zvalue function, zvalue args) {
    zint argCount = datSize(args);
    zvalue argsArray[argCount];

    datArrayFromList(argsArray, args);

    return datCall(function, argCount, argsArray);
}

/* Documented in header. */
zvalue datCall(zvalue function, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    zfunction caller = function->type->call;
    if (caller == NULL) {
        die("Attempt to call non-function.");
    }

    return caller(function, argCount, args);
}

/* Documented in header. */
zvalue datFnFrom(zint minArgs, zint maxArgs, zfunction function, zvalue state,
        zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = datAllocValue(DAT_Function, sizeof(DatFunction));
    DatFunction *info = fnInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->function = function;
    info->state = state;
    info->name = name;
    info->orderId = NULL;

    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue fnCall(zvalue function, zint argCount, const zvalue *args) {
    DatFunction *info = fnInfo(function);

    debugPush(callReporter, function);

    if (argCount < info->minArgs) {
        die("Too few arguments for function call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for function call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    zstackPointer save = datFrameStart();
    zvalue result = info->function(info->state, argCount, args);
    datFrameReturn(save, result);

    debugPop();
    return result;
}

/* Documented in header. */
static zorder fnOrder(zvalue v1, zvalue v2) {
    return datOrder(fnOrderId(v1), fnOrderId(v2));
}

/* Documented in header. */
static zvalue Function_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue function = args[0];
    DatFunction *info = fnInfo(function);

    datMark(info->state);
    datMark(info->name);
    datMark(info->orderId);

    return NULL;
}

/* Documented in header. */
void datBindFunction(void) {
    datGenBindCore(genGcMark, DAT_Function, Function_gcMark, NULL);
}

/* Documented in header. */
static DatType INFO_Function = {
    .name = "Function",
    .call = fnCall,
    .order = fnOrder
};
ztype DAT_Function = &INFO_Function;
