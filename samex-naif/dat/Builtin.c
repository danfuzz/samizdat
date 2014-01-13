/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Builtin Functions
 */

#include "impl.h"
#include "type/Builtin.h"
#include "type/Generic.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Value.h"


/*
 * Private Definitions
 */

/**
 * Builtin function structure.
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

    /** The builtin's name, if any. Used when producing stack traces. */
    zvalue name;
} BuiltinInfo;

/**
 * Gets a pointer to the value's info.
 */
static BuiltinInfo *getInfo(zvalue builtin) {
    return pbPayload(builtin);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue builtinCall(zvalue builtin, zint argCount, const zvalue *args) {
    BuiltinInfo *info = getInfo(builtin);

    if (argCount < info->minArgs) {
        die("Too few arguments for builtin call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for builtin call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    return info->function(argCount, args);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue makeBuiltin(zint minArgs, zint maxArgs, zfunction function,
        zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    zvalue result = pbAllocValue(TYPE_Builtin, sizeof(BuiltinInfo));
    BuiltinInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->function = function;
    info->name = name;

    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Builtin, call) {
    // The first argument is the builtin per se, and the rest are the
    // arguments to call it with.
    return builtinCall(args[0], argCount - 1, &args[1]);
}

/* Documented in header. */
METH_IMPL(Builtin, canCall) {
    zvalue builtin = args[0];
    zvalue value = args[1];
    BuiltinInfo *info = getInfo(builtin);

    return (info->maxArgs >= 1) ? value : NULL;
}

/* Documented in header. */
METH_IMPL(Builtin, debugString) {
    zvalue builtin = args[0];
    BuiltinInfo *info = getInfo(builtin);
    zvalue nameString = (info->name == NULL)
        ? stringFromUtf8(-1, "(unknown)")
        : GFN_CALL(debugString, info->name);

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@(Builtin "),
        nameString,
        stringFromUtf8(-1, ")"));
}

/* Documented in header. */
METH_IMPL(Builtin, gcMark) {
    zvalue builtin = args[0];
    BuiltinInfo *info = getInfo(builtin);

    pbMark(info->name);
    return NULL;
}

/* Documented in header. */
METH_IMPL(Builtin, nameOf) {
    zvalue builtin = args[0];
    BuiltinInfo *info = getInfo(builtin);

    return info->name;
}

/** Initializes the module. */
MOD_INIT(Builtin) {
    MOD_USE(Function);
    MOD_USE(OneOff);

    // Note: The `typeSystem` module initializes `TYPE_Builtin`.

    METH_BIND(Builtin, call);
    METH_BIND(Builtin, canCall);
    METH_BIND(Builtin, debugString);
    METH_BIND(Builtin, gcMark);
    METH_BIND(Builtin, nameOf);
}

/* Documented in header. */
zvalue TYPE_Builtin = NULL;
