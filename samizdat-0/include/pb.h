/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Lowest-layer plumbing and data structures
 */

#ifndef _PB_H_
#define _PB_H_

#include "ztype.h"


/*
 * C Types and macros
 */

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct PbHeader *zvalue;

/**
 * Prototype for an underlying C function corresponding to an in-model
 * function (value of type `Function`).
 */
typedef zvalue (*zfunction)(zint argCount, const zvalue *args);

/** Type for local value stack pointers. */
typedef const zvalue *zstackPointer;

enum {
    /**
     * (Private) Size of the `PbHeader` struct; used so that `pbOffset`
     * can be an inline function.
     */
    PB_HEADER_SIZE =
        (sizeof(zvalue) * 3) + (sizeof(int32_t) * 2) + sizeof(zint)
};


/*
 * Initialization Declarations
 */

/**
 * Initializes the `pb` module.
 */
void pbInit(void);


/*
 * Assertion Declarations
 */

/**
 * Asserts that the given size accommodates accessing the `n`th element.
 * This includes asserting that `n >= 0`. Note that all non-negative `n`
 * are valid for accessing ints (their size notwithstanding).
 */
void assertNth(zint size, zint n);

/**
 * Like `assertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void assertNthOrSize(zint size, zint n);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * for a value of the given size.
 */
void assertSliceRange(zint size, zint start, zint end);

/**
 * Asserts that the given value is a valid `zvalue` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void assertValid(zvalue value);

/**
 * Like `assertValid` except that `NULL` is accepted too.
 */
void assertValidOrNull(zvalue value);


/*
 * Memory Management Declarations
 */

/**
 * Allocates a value, assigning it the given type, and sizing the memory
 * to include the given amount of extra bytes as raw payload data.
 * The resulting value is added to the live reference stack.
 */
zvalue pbAllocValue(zvalue type, zint extraBytes);

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

/**
 * Forces a gc.
 */
void pbGc(void);

/**
 * Marks the given value as "immortal." It is considered a root and
 * will never get freed.
 */
void pbImmortalize(zvalue value);

/**
 * Marks a value during garbage collection. This in turn calls a type-specific
 * mark function when appropriate and may recurse arbitrarily. It is valid
 * to pass `NULL` to this, but no other non-values are acceptable.
 */
void pbMark(zvalue value);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
inline void *pbPayload(zvalue value) {
    return (void *) (((char *) value) + PB_HEADER_SIZE);
}

#endif
