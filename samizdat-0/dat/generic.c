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
 * Generic function structure.
 */
typedef struct {
    /** Default function, if any. May be `NULL`. */
    zvalue defaultFunction;

    /** Map from first-argument types to corresponding functions. */
    zvalue map;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Uniqlet to use for ordering comparisons. */
    zvalue orderToken;
} DatGeneric;

/**
 * Gets a pointer to the value's info.
 */
static DatGeneric *genInfo(zvalue generic) {
    return datPayload(generic);
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue name = genInfo((zvalue) state)->name;

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
zvalue datGenApply(zvalue generic, zvalue args) {
    zint argCount = datSize(args);
    zvalue argsArray[argCount];

    datArrayFromList(argsArray, args);

    return datGenCall(generic, argCount, argsArray);
}

/* Documented in header. */
zvalue datGenCall(zvalue generic, zint argCount, const zvalue *args) {
    datAssertGeneric(generic);

    if (argCount < 1) {
        die("Invalid argument count for generic call: %lld", argCount);
    } else if (args == NULL) {
        die("Generic call argument inconsistency.");
    }

    DatGeneric *info = genInfo(generic);

    debugPush(callReporter, generic);

    // TODO:
    // zvalue function = datMapGet(info->map, ... something ...);

    // zstackPointer save = datFrameStart();
    // zvalue result = info->function(info->state, argCount, args);
    // datFrameReturn(save, result);

    debugPop();

    // return result;
    die("TODO");
}


/*
 * Type binding
 */

/* Documented in header. */
static void genGcMark(zvalue function) {
    DatGeneric *info = genInfo(function);

    datMark(info->defaultFunction);
    datMark(info->map);
    datMark(info->name);
    datMark(info->orderToken);
}

/* Documented in header. */
static zorder genOrder(zvalue v1, zvalue v2) {
    return datOrder(genInfo(v1)->orderToken, genInfo(v2)->orderToken);
}

/* Documented in header. */
static DatType INFO_Generic = {
    .id = DAT_GENERIC,
    .name = "Generic",
    .sizeOf = NULL,
    .gcMark = genGcMark,
    .gcFree = NULL,
    .eq = NULL,
    .order = genOrder
};
ztype DAT_Generic = &INFO_Generic;
