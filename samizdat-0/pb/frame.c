/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/**
 * Stack of references. This is what is scanned in lieu of scanning
 * the "real" stack during gc.
 */
static zvalue stack[PB_MAX_STACK];

/** Current stack size. */
static zint stackSize;


/*
 * Module Definitions
 */

zint markFrameStack(void) {
    for (zint i = 0; i < stackSize; i++) {
        pbMark(stack[i]);
    }

    return stackSize;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zstackPointer pbFrameStart(void) {
    return &stack[stackSize];
}

/* Documented in header. */
void pbFrameAdd(zvalue value) {
    if (stackSize >= PB_MAX_STACK) {
        // As a hail-mary, do a forced gc and then clear the value stack, in
        // the hope that a gc won't end up being done while producing the
        // dying stack trace.
        pbGc();
        stackSize = 0;
        die("Value stack overflow.");
    }

    stack[stackSize] = value;
    stackSize++;
}

/* Documented in header. */
void pbFrameReset(zstackPointer savedStack, zvalue stackedValue) {
    // The difference between this function and `pbFrameReturn` is
    // one of intent, even though the implementation is (blatantly)
    // identical.
    pbFrameReturn(savedStack, stackedValue);
}

/* Documented in header. */
void pbFrameReturn(zstackPointer savedStack, zvalue returnValue) {
    zint returnSize = savedStack - stack;

    if (returnSize > stackSize) {
        die("Cannot return to deeper frame.");
    }

    stackSize = returnSize;

    if (returnValue != NULL) {
        pbFrameAdd(returnValue);
    }
}
