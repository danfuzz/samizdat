/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * In-model Generic Functions
 */

#include "impl.h"
#include "zlimits.h"


/*
 * Private Definitions
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

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Bindings from type to function, keyed off of type sequence number. */
    zvalue functions[PB_MAX_TYPES];
} GenericInfo;

/**
 * Gets a pointer to the value's info.
 */
static GenericInfo *getInfo(zvalue generic) {
    return pbPayload(generic);
}

/**
 * Find the binding for a given type.
 */
static zvalue findByTrueType(zvalue generic, zvalue type) {
    GenericInfo *info = getInfo(generic);

    for (/*type*/; type != NULL; type = typeParent(type)) {
        zvalue result = info->functions[indexFromType(type)];
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    return pbDebugString((zvalue) state);
}


/*
 * Module Definitions
 */

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

    zvalue function = genericFind(generic, args[0]);

    if (function == NULL) {
        die("No type binding found for generic: %s", pbDebugString(generic));
    }

    return funCall(function, argCount, args);
}

/* Documented in header. */
zvalue genericFind(zvalue generic, zvalue value) {
    return findByTrueType(generic, trueTypeOf(value));
}

/* Documented in header. */
zvalue genericFindByIndex(zvalue generic, zint index) {
    return getInfo(generic)->functions[index];
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void genericBindCore(zvalue generic, zvalue type, zfunction function) {
    assertHasType(generic, TYPE_Generic);

    GenericInfo *info = getInfo(generic);
    zint index = indexFromType(type);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic.");
    }

    info->functions[index] =
        makeFunction(info->minArgs, info->maxArgs, function, info->name);
}

/* Documented in header. */
void genericSeal(zvalue generic) {
    assertHasType(generic, TYPE_Generic);
    getInfo(generic)->sealed = true;
}

/* Documented in header. */
zvalue makeGeneric(zint minArgs, zint maxArgs, zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Generic, sizeof(GenericInfo));
    GenericInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->sealed = false;
    info->name = name;

    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Generic, call) {
    // The first argument is the generic per se, and the rest are the
    // arguments to call it with.
    return genericCall(args[0], argCount - 1, &args[1]);
}

/* Documented in header. */
METH_IMPL(Generic, canCall) {
    zvalue generic = args[0];
    zvalue value = args[1];
    GenericInfo *info = getInfo(generic);

    return (genericFind(generic, value) != NULL) ? value : NULL;
}

/* Documented in header. */
METH_IMPL(Generic, debugString) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);

    zvalue result = stringFromUtf8(-1, "@(Generic ");

    if (info->name != NULL) {
        result = stringAdd(result, GFN_CALL(debugString, info->name));
    } else {
        result = stringAdd(result, stringFromUtf8(-1, "(unknown)"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
METH_IMPL(Generic, gcMark) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);

    pbMark(info->name);
    for (zint i = 0; i < PB_MAX_TYPES; i++) {
        pbMark(info->functions[i]);
    }

    return NULL;
}

/* Documented in header. */
void pbBindGeneric(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Generic`.

    METH_BIND(Generic, call);
    METH_BIND(Generic, canCall);
    METH_BIND(Generic, debugString);
    METH_BIND(Generic, gcMark);
}

/* Documented in header. */
zvalue TYPE_Generic = NULL;
