/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * In-model Functions
 */

#include "impl.h"

#include <string.h>


/*
 * Helper definitions
 */

/**
 * Regular (non-generic) function structure.
 */
typedef struct {
    /** C function to call. */
    zfunction function;

    /** Closure state. */
    zvalue state;

    /** The function's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Uniqlet to use for ordering comparisons. */
    zvalue orderToken;
} DatFunction;

/**
 * Gets a pointer to the value's info.
 */
static DatFunction *fnInfo(zvalue function) {
    return datPayload(function);
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
zvalue datFnFrom(zfunction function, zvalue state, zvalue name) {
    zvalue result = datAllocValue(DAT_Function, sizeof(DatFunction));
    DatFunction *info = fnInfo(result);

    info->function = function;
    info->state = state;
    info->name = name;
    info->orderToken = datUniqlet();

    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue fnCall(zvalue function, zint argCount, const zvalue *args) {
    DatFunction *info = fnInfo(function);

    debugPush(callReporter, function);

    zstackPointer save = datFrameStart();
    zvalue result = info->function(info->state, argCount, args);
    datFrameReturn(save, result);

    debugPop();
    return result;
}

/* Documented in header. */
static void fnGcMark(zvalue function) {
    DatFunction *info = fnInfo(function);

    datMark(info->state);
    datMark(info->name);
    datMark(info->orderToken);
}

/* Documented in header. */
static zorder fnOrder(zvalue v1, zvalue v2) {
    return datOrder(fnInfo(v1)->orderToken, fnInfo(v2)->orderToken);
}

/* Documented in header. */
void datBindFunction(void) {
    // Nothing to do here...yet.
}

/* Documented in header. */
static DatType INFO_Function = {
    .name = "Function",
    .call = fnCall,
    .gcMark = fnGcMark,
    .gcFree = NULL,
    .eq = NULL,
    .order = fnOrder
};
ztype DAT_Function = &INFO_Function;
