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

/* Documented in header. */
void modUse(const char *name, zmodStatus *status, zmodInitFunction func) {
    switch (*status) {
        case MOD_UNINITIALIZED: {
            *status = MOD_INITIALIZING;
            func();
            if (*status != MOD_INITIALIZING) {
                die("Unexpected change in status for module: %s", name);
            }
            *status = MOD_INITIALIZED;
            break;
        }

        case MOD_INITIALIZING: {
            die("Circular dependency with module: %s", name);
        }

        case MOD_INITIALIZED: {
            // All good; nothing to do.
            break;
        }
    }
}
