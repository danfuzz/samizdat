/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "function.h"
#include "util.h"

#include <stdlib.h>

enum {
    /** Maximum number of entries in a function registry */
    MAX_REGISTRY_SIZE = 100000
};

/**
 * Representation of a function in a registry.
 */
typedef struct {
    /** The uniqlet identifier. */
    zvalue id;

    /** The low-level callable function. */
    zfunction function;

    /** Arbitrary closure state. */
    void *state;
}
Function;

/* Documented in header. */
typedef struct FunctionRegistry {
    /** The entries. */
    Function entries[MAX_REGISTRY_SIZE];

    /** Count of entries added. */
    zint size;
}
FunctionRegistry;


/*
 * Helper functions
 */

/**
 * Finds the entry with the given id. Note: This relies on uniqlets
 * sorting in the order they were created.
 */
static Function *findEntry(zfunreg reg, zvalue id) {
    Function *entries = reg->entries;
    zint min = 0;
    zint max = reg->size - 1;

    while (min <= max) {
        zint guess = (min + max) / 2;
        switch (samCompare(id, entries[guess].id)) {;
            case SAM_IS_LESS: max = guess - 1; break;
            case SAM_IS_MORE: min = guess + 1; break;
            default:          return &entries[guess];
        }
    }

    samDie("No such function.");
}


/*
 * Module functions
 */

/* Documented in header. */
zfunreg funNew(void) {
    // TODO: Make samAlloc() accessible from here, and use it.

    zfunreg reg = malloc(sizeof(FunctionRegistry));
    reg->size = 0;

    return reg;
}

/* Documented in header. */
zvalue funAdd(zfunreg reg, zfunction function, void *state) {
    if (reg->size == MAX_REGISTRY_SIZE) {
        samDie("Too many functions.");
    }

    Function *entry = &reg->entries[reg->size];

    entry->id = samUniqlet();
    entry->function = function;
    entry->state = state;
    reg->size++;

    return entry->id;
}

/* Documented in header. */
zvalue funCall(zfunreg reg, zvalue id, zint argCount, const zvalue *args) {
    Function *entry = findEntry(reg, id);

    return entry->function(entry->state, argCount, args);
}
