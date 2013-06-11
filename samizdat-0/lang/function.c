/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "lang.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Representation of an in-language function.
 */
typedef struct {
    /** The low-level callable function. */
    zfunction function;

    /** Arbitrary closure state. */
    zvalue state;
} Function;

/**
 * Marks the function's state for garbage collection.
 */
static void functionMark(void *state) {
    Function *function = state;
    datMark(function->state);
}

/**
 * Frees the function's state.
 */
static void functionFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table. */
static DatUniqletDispatch FUNCTION_DISPATCH = {
    functionMark,
    functionFree
};


/*
 * Exported functions
 */

/* Documented in header. */
zvalue langDefineFunction(zfunction function, zvalue state) {
    Function *entry = utilAlloc(sizeof(Function));

    entry->function = function;
    entry->state = state;

    return datUniqletWith(&FUNCTION_DISPATCH, entry);
}

/* Documented in header. */
zvalue langCall(zvalue functionId, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    Function *entry = datUniqletGetState(functionId, &FUNCTION_DISPATCH);

    zstackPointer save = datFrameStart();
    zvalue result = entry->function(entry->state, argCount, args);
    datFrameReturn(save, result);
    return result;
}

/* Documented in header. */
zvalue langApply(zvalue functionId, zvalue args) {
    zint argCount = datSize(args);
    zvalue argsArray[argCount];

    datArrayFromList(argsArray, args);

    zstackPointer save = datFrameStart();
    zvalue result = langCall(functionId, argCount, argsArray);
    datFrameReturn(save, result);
    return result;
}
