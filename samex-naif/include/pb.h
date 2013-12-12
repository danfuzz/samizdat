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

#include "module.h"
#include "ztype.h"

#include "pb/type.h"  // Types (must be included before other `pb` stuff).
#include "pb/frame.h" // Frame (stack reference) management.


/*
 * Assertion Declarations
 */

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
 * Code Loading Declarations
 */

/**
 * Loads and evaluates (runs) a native binary module, passing the given
 * `context` argument to its `eval` function. `path` indicates the filesystem
 * path to the module (as a flat string), which must be absolute. This
 * function returns whatever was returned by the `eval` function.
 */
zvalue pbEvalBinary(zvalue context, zvalue flatPath);


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
