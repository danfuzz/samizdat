/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Frame (stack references) management
 */

#ifndef _PB_FRAME_H_
#define _PB_FRAME_H_


/**
 * Adds an item to the current frame. This is only necessary to call when
 * a reference gets "detached" from a live structure (via mutation), which
 * is to say, rarely.
 */
void pbFrameAdd(zvalue value);

/**
 * Indicates the start of a new frame of references on the stack.
 * The return value can subsequently be passed to `pbFrameEnd` to
 * indicate that this frame is no longer active.
 */
zstackPointer pbFrameStart(void);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * should be reset to a state that *only* includes the given value
 * (or is totally reset if `stackedValue` is `NULL`).
 */
void pbFrameReset(zstackPointer savedStack, zvalue stackedValue);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * is no longer active. If the given additional value is non-`NULL` it is
 * added to the frame being "returned" to. It is valid to return to any
 * frame above the current one, not just the immediately-previous frame;
 * non-immediate return can happen during a nonlocal exit.
 */
void pbFrameReturn(zstackPointer savedStack, zvalue returnValue);

#endif
