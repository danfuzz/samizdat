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
    zfunction defaultFunction;

    /** Whether the generic is sealed (unwilling to add bindings). */
    bool sealed;

    /** The generic's name, if any. Used when producing stack traces. */
    zvalue name;

    /** Id to use for ordering comparisons. */
    zint orderId;

    /** Bindings from type to function, keyed off of type sequence number. */
    zfunction functions[DAT_MAX_TYPES];
} DatGeneric;

/**
 * Gets a pointer to the value's info.
 */
static DatGeneric *gfnInfo(zvalue generic) {
    return datPayload(generic);
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    return datDebugString((zvalue) state);
}


/*
 * Module functions
 */

/* Documented in header. */
zfunction datGfnFind(zvalue generic, zvalue value) {
    // TODO: Dispatch is currently on the core type. It should be able
    // to handle derived types too. It's not as simple as just calling
    // `datTypeOf` on the value, though: (1) That function itself is
    // generic, and (2) the default implementations of many generics
    // will have to be adjusted.

    DatGeneric *info = gfnInfo(generic);
    zfunction result = info->functions[datIndexFromType(value->type)];

    return (result != NULL) ? result : info->defaultFunction;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datCall(zvalue function, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    debugPush(callReporter, function);
    zstackPointer save = datFrameStart();

    zfunction caller =
        gfnInfo(GFN_call)->functions[~function->type->seqNumCompl];

    if (caller == NULL) {
        die("Attempt to call non-function.");
    }

    zvalue result = caller(function, argCount, args);

    datFrameReturn(save, result);
    debugPop();

    return result;
}

/* Documented in header. */
void datGfnBindCore(zvalue generic, ztype type, zfunction function) {
    datAssertGeneric(generic);

    DatGeneric *info = gfnInfo(generic);
    zint index = datIndexFromType(type);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic.");
    }

    info->functions[index] = function;
}

/* Documented in header. */
void datGfnBindCoreDefault(zvalue generic, zfunction function) {
    datAssertGeneric(generic);

    DatGeneric *info = gfnInfo(generic);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->defaultFunction != NULL) {
        die("Default already bound in generic.");
    }

    info->defaultFunction = function;
}

/* Documented in header. */
zvalue datGfnFrom(zint minArgs, zint maxArgs, zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = datAllocValue(DAT_Generic, sizeof(DatGeneric));
    DatGeneric *info = gfnInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->defaultFunction = NULL;
    info->sealed = false;
    info->name = name;
    info->orderId = pbOrderId();

    return result;
}

/* Documented in header. */
void datGfnSeal(zvalue generic) {
    datAssertGeneric(generic);
    gfnInfo(generic)->sealed = true;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Generic_call(zvalue generic, zint argCount, const zvalue *args) {
    DatGeneric *info = gfnInfo(generic);

    if (argCount < info->minArgs) {
        die("Too few arguments for generic call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for generic call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    zfunction function = datGfnFind(generic, args[0]);

    if (function == NULL) {
        die("No type binding found for generic.");
    }

    return function(NULL, argCount, args);
}

/* Documented in header. */
static zvalue Generic_debugString(zvalue state,
        zint argCount, const zvalue *args) {
    zvalue generic = args[0];
    DatGeneric *info = gfnInfo(generic);

    zvalue result = datStringFromUtf8(-1, "@(Generic ");

    if (info->name != NULL) {
        result = datStringAdd(result, datCall(GFN_debugString, 1, &info->name));
    } else {
        result = datStringAdd(result, datStringFromUtf8(-1, "(unknown)"));
    }

    result = datStringAdd(result, datStringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
static zvalue Generic_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue generic = args[0];
    DatGeneric *info = gfnInfo(generic);

    datMark(info->name);

    return NULL;
}

/* Documented in header. */
static zvalue Generic_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    return (gfnInfo(v1)->orderId < gfnInfo(v2)->orderId) ? DAT_NEG1 : DAT_1;
}

/* Documented in header. */
void pbBindGeneric(void) {
    datGfnBindCore(GFN_call,        DAT_Generic, Generic_call);
    datGfnBindCore(GFN_debugString, DAT_Generic, Generic_debugString);
    datGfnBindCore(GFN_gcMark,      DAT_Generic, Generic_gcMark);
    datGfnBindCore(GFN_order,       DAT_Generic, Generic_order);
}

/* Documented in header. */
static DatType INFO_Generic = {
    .name = "Generic"
};
ztype DAT_Generic = &INFO_Generic;
