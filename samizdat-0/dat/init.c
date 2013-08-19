/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "impl.h"
#include "type/Box.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
void datInit(void) {
    if (DAT_NULL_BOX != NULL) {
        return;
    }

    zstackPointer save = pbFrameStart();

    pbInit();
    datBindBox();
    datBindList();
    datBindMap();
    datBindMapCache();

    pbFrameReturn(save, NULL);
}

