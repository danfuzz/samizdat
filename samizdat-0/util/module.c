/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Simple C Module System
 */

#include "module.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Record of a pending initialization. First three members are the same as the
 * arguments to `modUse` (see which).
 */
typedef struct PendingInit {
    const char *name;
    zmodStatus *status;
    zmodInitFunction func;

    /** Next one in the chain. */
    struct PendingInit *next;
} PendingInit;

/**
 * List of modules that should be initialized after the current initialization
 * is complete.
 */
static PendingInit *thePendingInits = NULL;

/**
 * Services the pending init stack, draining it.
 */
static void servicePendingInits(void) {
    // Note: `modUse()` can end up adding back to the stack, so we always
    // have to leave the it in a consistent state.

    while (thePendingInits != NULL) {
        PendingInit *one = thePendingInits;
        thePendingInits = one->next;

        switch (*(one->status)) {
            case MOD_UNINITIALIZED: {
                *(one->status) = MOD_INITIALIZING;
                one->func();
                if (*(one->status) != MOD_INITIALIZING) {
                    die("Unexpected change in status for module: %s",
                        one->name);
                }
                *(one->status) = MOD_INITIALIZED;
                break;
            }

            case MOD_INITIALIZING: {
                die("Circular dependency with module: %s", (one->name));
            }

            case MOD_INITIALIZED: {
                // All good; nothing to do.
                break;
            }
        }

        utilFree(one);
    }
}

/* Documented in header. */
void modUseNext(const char *name, zmodStatus *status, zmodInitFunction func) {
    if (*status != MOD_UNINITIALIZED) {
        // It's already initialized or in-progress.
        return;
    }

    PendingInit *info = utilAlloc(sizeof(PendingInit));
    info->name = name;
    info->status = status;
    info->func = func;
    info->next = thePendingInits;

    thePendingInits = info;
}

/* Documented in header. */
void modUse(const char *name, zmodStatus *status, zmodInitFunction func) {
    // First, add the given info as a pending init.
    modUseNext(name, status, func);

    // And then, service the pending stack, emptying it.
    servicePendingInits();
}
