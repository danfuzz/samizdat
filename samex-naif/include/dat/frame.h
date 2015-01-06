// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Frame (stack references) management
//

#ifndef _DAT_FRAME_H_
#define _DAT_FRAME_H_


//
// Private Definitions
//

/** Base of the stack (builds forward in memory). */
extern zstackPointer frameStackBase;

/** Points at the location for the next `add`. */
extern zstackPointer frameStackTop;

/** Limit of the stack (highest possible value for `frameStackTop`). */
extern zstackPointer frameStackLimit;

/** Indicates a fatal error. */
void datFrameError(const char *message);


//
// Public Definitions
//

/**
 * Adds an item to the current frame and returns it. This is only necessary to
 * call when a reference gets "detached" from a structure (e.g. returning
 * an element out of a collection, where it is not known that the collection
 * will remain live after the call) and either returned from a C function
 * (but not an in-model function or method call) or stored in a structure that
 * GC won't immediately find.
 */
inline zvalue datFrameAdd(zvalue value) {
    if (value == NULL) {
        return NULL;
    } else if (frameStackTop == frameStackLimit) {
        datFrameError("Value stack overflow.");
    }

    *frameStackTop = value;
    frameStackTop++;
    return value;
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
    datFrameAdd(returnValue);
}

#endif
