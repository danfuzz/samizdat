// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Builtin Functions
//

#include "type/Builtin.h"
#include "type/String.h"
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
zvalue builtinCall(zvalue builtin, zarray args) {
    BuiltinInfo *info = getInfo(builtin);

    if (args.size < info->minArgs) {
        die("Too few arguments for builtin call: %d, min %d",
            args.size, info->minArgs);
    } else if (args.size > info->maxArgs) {
        die("Too many arguments for builtin call: %d, max %d",
            args.size, info->maxArgs);
    }

    return info->function(builtin, args);
}


//
// Exported Definitions
//

// Documented in header.
zvalue makeBuiltin(zint minArgs, zint maxArgs, zfunction function,
        zint stateSize, zvalue name) {
    if ((minArgs < 0) ||
        ((maxArgs != -1) && (maxArgs < minArgs))) {
        die("Invalid `minArgs` / `maxArgs`: %d, %d", minArgs, maxArgs);
    }

    if (stateSize < 0) {
        die("Invalid `stateSize`: %d", stateSize);
    }

    if (name != NULL) {
        assertHasClass(name, CLS_Symbol);
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
        return (BuiltinState) {0, NULL};
    } else {
        return (BuiltinState) {size, info->state};
    }
}


//
// Class Definition
//

// Documented in spec.
METH_IMPL_rest(Builtin, call, args) {
    return builtinCall(ths, args);
}

// Documented in spec.
METH_IMPL_0(Builtin, debugSymbol) {
    BuiltinInfo *info = getInfo(ths);
    return info->name;
}

// Documented in header.
METH_IMPL_0(Builtin, gcMark) {
    BuiltinInfo *info = getInfo(ths);
    zvalue *state = info->state;

    datMark(info->name);

    for (zint i = 0; i < info->stateSize; i++) {
        datMark(state[i]);
    }

    return NULL;
}

// Documented in header.
void bindMethodsForBuiltin(void) {
    classBindMethods(CLS_Builtin,
        NULL,
        METH_TABLE(
            METH_BIND(Builtin, call),
            METH_BIND(Builtin, debugSymbol),
            METH_BIND(Builtin, gcMark)));
}

/** Initializes the module. */
MOD_INIT(Builtin) {
    MOD_USE(Core);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForBuiltin()`.
}

// Documented in header.
zvalue CLS_Builtin = NULL;
