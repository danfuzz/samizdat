/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/** Key for access to function-bearing uniqlets. Fun self-reference! */
static void *functionKey = &functionKey;

/**
 * Representation of an in-language function.
 */
typedef struct {
    /** The low-level callable function. */
    zfunction function;

    /** Arbitrary closure state. */
    void *state;
} Function;


/*
 * Exported functions
 */

/* Documented in header. */
zvalue langDefineFunction(zfunction function, void *state) {
    Function *entry = zalloc(sizeof(Function));

    entry->function = function;
    entry->state = state;

    return datUniqletWith(functionKey, entry);
}

/* Documented in header. */
zvalue langCall(zvalue functionId, zint argCount, const zvalue *args) {
    if (argCount < 0) {
        die("Invalid argument count for function call: %lld", argCount);
    } else if ((argCount != 0) && (args == NULL)) {
        die("Function call argument inconsistency.");
    }

    Function *entry = datUniqletGetValue(functionId, functionKey);

    return entry->function(entry->state, argCount, args);
}

/* Documented in header. */
zvalue langApply(zvalue functionId, zvalue args) {
    zint argCount = datSize(args);
    zvalue argsArray[argCount];

    datArrayFromListlet(argsArray, args);
    return langCall(functionId, argCount, argsArray);
}
