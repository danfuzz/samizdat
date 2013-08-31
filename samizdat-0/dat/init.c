/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"


/*
 * Exported Definitions
 */

/** Initializes the module. */
MOD_INIT(dat) {
    zstackPointer save = pbFrameStart();

    MOD_USE(pb);
    MOD_USE(Generator); // Inits everything (indirectly).

    pbFrameReturn(save, NULL);
}

