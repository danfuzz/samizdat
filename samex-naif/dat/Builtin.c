// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Builtin Functions
//

#include "type/Builtin.h"
#include "type/Value.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Builtin function structure.
 */
typedef struct {
    /** Minimum argument count. Always `>= 0`. */
    zint minArgs;

    /** Maximum argument count. Always `>= minArgs`. */
    zint maxArgs;

    /** C function to call. */
    zfunction function;

    /** The count of mutable slots of state. Always `>= 0`. */
    zint stateSize;

    /** The builtin's name, if any. Used when producing stack traces. */
    zvalue name;

    /** The mutable state (if any). */
    zvalue state[/*stateSize*/];
} BuiltinInfo;

/**
 * Gets a pointer to the value's info.
 */
static BuiltinInfo *getInfo(zvalue builtin) {
    return datPayload(builtin);
}


//
// Module Definitions
//

// Documented in header.
zvalue builtinCall(zvalue builtin, zint argCount, const zvalue *args) {
    BuiltinInfo *info = getInfo(builtin);

    if (argCount < info->minArgs) {
        die("Too few arguments for builtin call: %lld, min %lld",
            argCount, info->minArgs);
    } else if (argCount > info->maxArgs) {
        die("Too many arguments for builtin call: %lld, max %lld",
            argCount, info->maxArgs);
    }

    return info->function(builtin, argCount, args);
}


//
// Exported Definitions
//

// Documented in header.
zvalue makeBuiltin(zint minArgs, zint maxArgs, zfunction function,
        zint stateSize, zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %lld, %lld", minArgs, maxArgs);
    }

    if (stateSize < 0) {
        die("Invalid `stateSize`: %lld", stateSize);
    }

    zvalue result = datAllocValue(CLS_Builtin,
        sizeof(BuiltinInfo) + stateSize * sizeof(zvalue));
    BuiltinInfo *info = getInfo(result);

    info->minArgs = minArgs;
    info->maxArgs = (maxArgs != -1) ? maxArgs : INT64_MAX;
    info->function = function;
    info->stateSize = stateSize;
    info->name = name;

    return result;
}

// Documented in header.
BuiltinState builtinGetState(zvalue builtin) {
    assertHasClass(builtin, CLS_Builtin);

    BuiltinInfo *info = getInfo(builtin);
    zint size = info->stateSize;

    if (size == 0) {
        BuiltinState result = { 0, NULL };
        return result;
    } else {
        BuiltinState result = { size, info->state };
        return result;
    }
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Builtin, call) {
    // The first argument is the builtin per se, and the rest are the
    // arguments to call it with.
    return builtinCall(args[0], argCount - 1, &args[1]);
}

// Documented in header.
METH_IMPL(Builtin, debugName) {
    zvalue builtin = args[0];
    BuiltinInfo *info = getInfo(builtin);

    return info->name;
}

// Documented in header.
METH_IMPL(Builtin, gcMark) {
    zvalue builtin = args[0];
    BuiltinInfo *info = getInfo(builtin);
    zvalue *state = info->state;

    datMark(info->name);

    for (zint i = 0; i < info->stateSize; i++) {
        datMark(state[i]);
    }

    return NULL;
}

/** Initializes the module. */
MOD_INIT(Builtin) {
    MOD_USE(Function);
    MOD_USE(OneOff);

    // Note: The `objectModel` module initializes `CLS_Builtin`.
    classBindMethods(CLS_Builtin,
        NULL,
        symbolTableFromArgs(
            SEL_METH(Builtin, call),
            SEL_METH(Builtin, debugName),
            SEL_METH(Builtin, gcMark),
            NULL));
}

// Documented in header.
zvalue CLS_Builtin = NULL;
