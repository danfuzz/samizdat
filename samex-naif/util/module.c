// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Simple C Module System
//

#include "module.h"
#include "util.h"


//
// Private Definitions
//

/**
 * Record of a pending initialization. First three members are the same as the
 * arguments to `modUse` (see which).
 */
typedef struct PendingInit {
    const char *name;
    zmodStatus *status;
    zmodInitFunction func;

    /** Previous one in the chain. */
    struct PendingInit *prev;

    /** Next one in the chain. */
    struct PendingInit *next;
} PendingInit;

/**
 * Queue of modules that should be initialized after the current initialization
 * is complete. Implemented as a circular double linked list.
 */
static PendingInit thePendingHead = {
    .prev = &thePendingHead,
    .next = &thePendingHead
};

/**
 * Services the pending init queue, draining it.
 */
static void servicePendingInits(void) {
    // Note: Running an init function can end up adding back to the queue,
    // so we always have to leave it in a consistent state.

    for (;;) {
        PendingInit *one = thePendingHead.next;
        if (one == &thePendingHead) {
            break;
        }

        one->next->prev = one->prev;
        one->prev->next = one->next;

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

// Documented in header.
void modUseNext(const char *name, zmodStatus *status, zmodInitFunction func) {
    if (*status != MOD_UNINITIALIZED) {
        // It's already initialized or in-progress.
        return;
    }

    PendingInit *one = utilAlloc(sizeof(PendingInit));
    one->name = name;
    one->status = status;
    one->func = func;
    one->next = &thePendingHead;
    one->prev = thePendingHead.prev;
    one->next->prev = one;
    one->prev->next = one;
}

// Documented in header.
void modUse(const char *name, zmodStatus *status, zmodInitFunction func) {
    // Save off the current pending init queue.
    PendingInit saveHead = thePendingHead;

    // Make a new queue with just the module we're asked to use, and service
    // the queue until empty.
    thePendingHead.prev = &thePendingHead;
    thePendingHead.next = &thePendingHead;
    modUseNext(name, status, func);
    servicePendingInits();

    // Restore the old queue.
    thePendingHead = saveHead;
}
