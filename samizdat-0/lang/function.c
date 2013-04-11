/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"

#include <stddef.h>


/** Key for access to function-bearing uniqlets. Fun self-reference! */
static void *functionKey = &functionKey;

/**
 * Representation of a function in a registry.
 */
typedef struct {
    /** The low-level callable function. */
    zfunction function;

    /** Arbitrary closure state. */
    void *state;
} Function;


/*
 * Module functions
 */

/* Documented in header. */
zvalue funDefine(zfunction function, void *state) {
    Function *entry = zalloc(sizeof(Function));

    entry->function = function;
    entry->state = state;

    return datUniqletWith(functionKey, entry);
}


/*
 * Exported functions
 */

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
