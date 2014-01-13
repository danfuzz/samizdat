/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * In-model Generic Functions
 */

#include "impl.h"
#include "type/Builtin.h"
#include "type/Generic.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
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

    /** Maximum argument count. Always `>= minArgs`. */
    zint maxArgs;

    /** Flags (restrictions on arguments). */
    zgenericFlags flags;

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
 * Find the function binding for a given type, including walking up the
 * parent type chain. Returns `NULL` if there is no binding.
 */
static zvalue findByTrueType(zvalue generic, zvalue type) {
    zvalue *functions = getInfo(generic)->functions;

    for (/*type*/; type != NULL; type = typeParent(type)) {
        zvalue result = functions[indexFromTrueType(type)];
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
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

    zgenericFlags flags = info->flags;
    if (flags & GFN_SAME_TYPE) {
        assertAllHaveSameType(argCount, args);
    }

    zvalue function = findByTrueType(generic, trueTypeOf(args[0]));

    if (function == NULL) {
        die("No binding found: %s(%s, ...)",
            valDebugString(generic), valDebugString(args[0]));
    }

    return funCall(function, argCount, args);
}

/* Documented in header. */
zvalue genericFindByIndex(zvalue generic, zint index) {
    return getInfo(generic)->functions[index];
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void genericBind(zvalue generic, zvalue typeOrName, zvalue function) {
    assertHasType(generic, TYPE_Generic);

    GenericInfo *info = getInfo(generic);
    zint index = typeIndex(typeOrName);

    if (info->sealed) {
        die("Sealed generic.");
    } else if (info->functions[index] != NULL) {
        die("Duplicate binding in generic: %s(%s, ...)",
            valDebugString(generic), valDebugString(typeOrName));
    }

    info->functions[index] = function;
}

/* Documented in header. */
void genericBindPrim(zvalue generic, zvalue typeOrName, zfunction function,
        const char *builtinName) {
    assertHasType(generic, TYPE_Generic);

    GenericInfo *info = getInfo(generic);
    zvalue name = (builtinName != NULL)
        ? stringFromUtf8(-1, builtinName)
        : info->name;
    zvalue builtin = makeBuiltin(info->minArgs, info->maxArgs, function, name);

    genericBind(generic, typeOrName, builtin);
}

/* Documented in header. */
void genericSeal(zvalue generic) {
    assertHasType(generic, TYPE_Generic);
    getInfo(generic)->sealed = true;
}

/* Documented in header. */
zvalue makeGeneric(zint minArgs, zint maxArgs, zgenericFlags flags,
        zvalue name) {
    if ((minArgs < 1) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Generic, sizeof(GenericInfo));
    GenericInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->flags = flags;
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

    return (findByTrueType(generic, trueTypeOf(value)) != NULL) ? value : NULL;
}

/* Documented in header. */
METH_IMPL(Generic, debugString) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);
    zvalue nameString = (info->name == NULL)
        ? stringFromUtf8(-1, "(unknown)")
        : GFN_CALL(debugString, info->name);

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@(Generic "),
        nameString,
        stringFromUtf8(-1, ")"));
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
METH_IMPL(Generic, nameOf) {
    zvalue generic = args[0];
    GenericInfo *info = getInfo(generic);

    return info->name;
}

/** Initializes the module. */
MOD_INIT(Generic) {
    MOD_USE(Function);
    MOD_USE(OneOff);

    // Note: The `typeSystem` module initializes `TYPE_Generic`.

    METH_BIND(Generic, call);
    METH_BIND(Generic, canCall);
    METH_BIND(Generic, debugString);
    METH_BIND(Generic, gcMark);
    METH_BIND(Generic, nameOf);
}

/* Documented in header. */
zvalue TYPE_Generic = NULL;
