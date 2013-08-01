/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * In-model Functions
 */

#include "impl.h"
#include "zlimits.h"

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

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Uniqlet to use for ordering comparisons. */
    zvalue orderToken;

    /** Bindings from type to function, keyed off of type sequence number. */
    zvalue functions[DAT_MAX_TYPES];
} DatGeneric;

/**
 * Gets a pointer to the value's info.
 */
static DatGeneric *genInfo(zvalue generic) {
    return datPayload(generic);
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
void datGenBindCore(zvalue generic, ztype type,
        zfunction function, zvalue state) {
    datAssertGeneric(generic);

    DatGeneric *info = genInfo(generic);
    zvalue functionValue = datFnFrom(function, state, info->name);
    zint index = datIndexFromType(type);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic.");
    }

    info->functions[index] = functionValue;
}

/* Documented in header. */
void datGenBindCoreDefault(zvalue generic, zfunction function, zvalue state) {
    datAssertGeneric(generic);

    DatGeneric *info = genInfo(generic);
    zvalue functionValue = datFnFrom(function, state, info->name);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->defaultFunction != NULL) {
        die("Default already bound in generic.");
    }

    info->defaultFunction = functionValue;
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

    // TODO: Dispatch is currently on the core type. It should be able
    // to handle derived types too. It's not as simple as just calling
    // `datTypeOf` on the value, though: (1) That function itself should
    // be generic at some point, and (2) the default implementations of
    // many generics will have to be adjusted.
    zvalue function = info->functions[datIndexFromType(args[0]->type)];

    if (function == NULL) {
        function = info->defaultFunction;
        if (function == NULL) {
            die("No type binding found for generic.");
        }
    }

    return datFnCall(function, argCount, args);
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
    datMark(info->name);
    datMark(info->orderToken);

    for (zint i = 0; i < DAT_MAX_TYPES; i++) {
        datMark(info->functions[i]);
    }
}

/* Documented in header. */
static zorder genOrder(zvalue v1, zvalue v2) {
    return datOrder(genInfo(v1)->orderToken, genInfo(v2)->orderToken);
}

/* Documented in header. */
void datBindGeneric(void) {
    // Nothing to do here...yet.
}

/* Documented in header. */
static DatType INFO_Generic = {
    .name = "Generic",
    .gcMark = genGcMark,
    .gcFree = NULL,
    .eq = NULL,
    .order = genOrder
};
ztype DAT_Generic = &INFO_Generic;
