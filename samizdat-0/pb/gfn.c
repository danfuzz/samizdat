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

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Id to use for ordering comparisons. */
    zint orderId;

    /** Bindings from type to function, keyed off of type sequence number. */
    zfunction functions[PB_MAX_TYPES];
} GenericInfo;

/**
 * Gets a pointer to the value's info.
 */
static GenericInfo *gfnInfo(zvalue generic) {
    return pbPayload(generic);
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    return pbDebugString((zvalue) state);
}


/*
 * Module functions
 */

/* Documented in header. */
zfunction gfnFind(zvalue generic, zvalue value) {
    GenericInfo *info = gfnInfo(generic);

    for (zvalue type = value->type; type != NULL; type = typeParent(type)) {
        zfunction result = info->functions[indexFromType(type)];
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}


/*
 * Exported functions
 */

/* Documented in header. */
void pbAssertGeneric(zvalue value) {
    assertHasType(value, TYPE_Generic);
}

/* Documented in header. */
zvalue fnCall(zvalue function, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    UTIL_TRACE_START(callReporter, function);
    zstackPointer save = pbFrameStart();

    zint index = indexFromType(function->type);
    zfunction caller = gfnInfo(GFN_call)->functions[index];

    if (caller == NULL) {
        die("Attempt to call non-function.");
    }

    zvalue result = caller(function, argCount, args);

    pbFrameReturn(save, result);
    UTIL_TRACE_END();

    return result;
}

/* Documented in header. */
void gfnBindCore(zvalue generic, zvalue type, zfunction function) {
    pbAssertGeneric(generic);

    GenericInfo *info = gfnInfo(generic);
    zint index = indexFromType(type);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic.");
    }

    info->functions[index] = function;
}

/* Documented in header. */
zvalue gfnFrom(zint minArgs, zint maxArgs, zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Generic, sizeof(GenericInfo));
    GenericInfo *info = gfnInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->sealed = false;
    info->name = name;
    info->orderId = pbOrderId();

    return result;
}

/* Documented in header. */
void gfnSeal(zvalue generic) {
    pbAssertGeneric(generic);
    gfnInfo(generic)->sealed = true;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Generic_call(zvalue generic, zint argCount, const zvalue *args) {
    GenericInfo *info = gfnInfo(generic);

    if (argCount < info->minArgs) {
        die("Too few arguments for generic call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for generic call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    zfunction function = gfnFind(generic, args[0]);

    if (function == NULL) {
        die("No type binding found for generic.");
    }

    return function(NULL, argCount, args);
}

/* Documented in header. */
static zvalue Generic_debugString(zvalue state,
        zint argCount, const zvalue *args) {
    zvalue generic = args[0];
    GenericInfo *info = gfnInfo(generic);

    zvalue result = stringFromUtf8(-1, "@(Generic ");

    if (info->name != NULL) {
        result = stringAdd(result, fnCall(GFN_debugString, 1, &info->name));
    } else {
        result = stringAdd(result, stringFromUtf8(-1, "(unknown)"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
static zvalue Generic_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue generic = args[0];
    GenericInfo *info = gfnInfo(generic);

    pbMark(info->name);

    return NULL;
}

/* Documented in header. */
static zvalue Generic_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (gfnInfo(v1)->orderId < gfnInfo(v2)->orderId) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void pbBindGeneric(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_Generic`.

    gfnBindCore(GFN_call,        TYPE_Generic, Generic_call);
    gfnBindCore(GFN_debugString, TYPE_Generic, Generic_debugString);
    gfnBindCore(GFN_gcMark,      TYPE_Generic, Generic_gcMark);
    gfnBindCore(GFN_order,       TYPE_Generic, Generic_order);
}

/* Documented in header. */
zvalue TYPE_Generic = NULL;
