// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "impl.h"


//
// Private Definitions
//

/** Actual stack memory. */
static zvalue theStack[DAT_MAX_STACK];

// Documented in header.
zstackPointer frameStackBase = theStack;

// Documented in header.
zstackPointer frameStackTop = theStack;

// Documented in header.
zstackPointer frameStackLimit = &theStack[DAT_MAX_STACK];

// Documented in header.
void datFrameError(const char *msg) {
    // As a hail-mary, do a forced gc and then clear the value stack, in
    // the hope that a gc won't end up being done while producing the
    // dying stack trace.
    datGc();
    frameStackTop = frameStackBase;

    die("%s", msg);
}


//
// Module Definitions
//

zint markFrameStack(void) {
    zint stackSize = frameStackTop - frameStackBase;

    for (int i = 0; i < stackSize; i++) {
        datMark(theStack[i]);
    }

    return stackSize;
}


//
// Exported Definitions
//

// Documented in header.
extern zstackPointer datFrameStart(void);

// Documented in header.
extern zvalue datFrameAdd(zvalue value);

// Documented in header.
extern void datFrameReturn(zstackPointer savedStack, zvalue returnValue);
