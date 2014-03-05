/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Frame (stack references) management
 */

#ifndef _DAT_FRAME_H_
#define _DAT_FRAME_H_


/*
 * Private Definitions
 */

/** Base of the stack (builds forward in memory). */
extern zstackPointer frameStackBase;

/** Points at the location for the next `add`. */
extern zstackPointer frameStackTop;

/** Limit of the stack (highest possible value for `frameStackTop`). */
extern zstackPointer frameStackLimit;

/** Indicates a fatal error. */
void datFrameError(const char *message);


/*
 * Public Definitions
 */

/**
 * Adds an item to the current frame. This is only necessary to call when
 * a reference gets "detached" from a live structure (via mutation), which
 * is to say, rarely.
 */
inline void datFrameAdd(zvalue value) {
    if (frameStackTop == frameStackLimit) {
        datFrameError("Value stack overflow.");
    }

    *frameStackTop = value;
    frameStackTop++;
}

/**
 * Indicates the start of a new frame of references on the stack.
 * The return value can subsequently be passed to `datFrameEnd` to
 * indicate that this frame is no longer active.
 */
inline zstackPointer datFrameStart(void) {
    return frameStackTop;
}

/**
 * Indicates that the frame whose start returned the given stack pointer
 * is no longer active. If the given additional value is non-`NULL` it is
 * added to the frame being "returned" to. It is valid to return to any
 * frame above the current one, not just the immediately-previous frame;
 * non-immediate return can happen during a nonlocal exit.
 */
inline void datFrameReturn(zstackPointer savedStack, zvalue returnValue) {
    if (savedStack > frameStackTop) {
        datFrameError("Cannot return to deeper frame.");
    }

    frameStackTop = savedStack;

    if (returnValue) {
        datFrameAdd(returnValue);
    }
}

/**
 * Indicates that the frame whose start returned the given stack pointer
 * should be reset to a state that *only* includes the given value
 * (or is totally reset if `stackedValue` is `NULL`).
 */
inline void datFrameReset(zstackPointer savedStack, zvalue stackedValue) {
    // The difference between this function and `datFrameReturn` is
    // one of intent, even though the implementation is (blatantly)
    // identical.
    datFrameReturn(savedStack, stackedValue);
}

#endif
