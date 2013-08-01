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
 * Generic function structure.
 */
typedef struct {
    /** Minimum argument count. Always `>= 1`. */
    zint minArgs;

    /**
     * Maximum argument count. Always `>= minArgs`.
     */
    zint maxArgs;

    /** Default function, if any. May be `NULL`. */
    zvalue defaultFunction;

    /** Map from first-argument types to corresponding functions. */
    zvalue map;

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

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
void datGenBind(zvalue generic, zvalue type, zvalue function) {
    datAssertGeneric(generic);
    datAssertFunction(function);

    DatGeneric *info = genInfo(generic);
    zvalue map = info->map;

    if (map == NULL) {
        info->map = datMapping(type, function);
    } else if (datMapGet(map, type) != NULL) {
        die("Duplicate binding in generic.");
    } else {
        info->map = datMapPut(map, type, function);
    }
}

/* Documented in header. */
void datGenBindCore(zvalue generic, ztype type,
        zfunction function, zvalue state) {
    datAssertGeneric(generic);

    DatGeneric *info = genInfo(generic);
    zvalue functionValue = datFnFrom(function, state, info->name);

    datGenBind(generic, datTypeFromZtype(type), functionValue);
}

/* Documented in header. */
void datGenBindDefault(zvalue generic, zvalue function) {
    datAssertGeneric(generic);
    datAssertFunction(function);

    DatGeneric *info = genInfo(generic);

    if (info->defaultFunction != NULL) {
        die("Default already bound in generic.");
    }

    info->defaultFunction = function;
}

/* Documented in header. */
zvalue datGenCall(zvalue generic, zint argCount, const zvalue *args) {
    datAssertGeneric(generic);

    DatGeneric *info = genInfo(generic);

    if (argCount < info->minArgs) {
        die("Too few arguments for generic call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too few arguments for generic call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (args == NULL) {
        die("Invalid argument pointer (NULL).");
    }

    zvalue function = datMapGet(info->map, datTypeOf(args[0]));

    debugPush(callReporter, generic);

    if (function == NULL) {
        function = info->defaultFunction;
        if (function == NULL) {
            die("No type binding found for generic.");
        }
    }

    zvalue result = datFnCall(function, argCount, args);

    debugPop();
    return result;
}

/* Documented in header. */
zvalue datGenFrom(zint minArgs, zint maxArgs, zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = datAllocValue(DAT_Generic, sizeof(DatGeneric));
    DatGeneric *info = genInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->defaultFunction = NULL;
    info->map = NULL;
    info->sealed = false;
    info->name = name;
    info->orderToken = datUniqlet();

    return result;
}

/* Documented in header. */
void datGenSeal(zvalue generic) {
    datAssertGeneric(generic);
    genInfo(generic)->sealed = true;
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
    .dataOf = NULL,
    .sizeOf = NULL,
    .typeOf = NULL,
    .gcMark = genGcMark,
    .gcFree = NULL,
    .eq = NULL,
    .order = genOrder
};
ztype DAT_Generic = &INFO_Generic;
