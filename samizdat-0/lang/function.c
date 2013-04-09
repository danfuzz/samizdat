/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"


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

/* Documented in header. */
zvalue funCall(zvalue id, zint argCount, const zvalue *args) {
    Function *entry = datUniqletGetValue(id, functionKey);

    return entry->function(entry->state, argCount, args);
}
