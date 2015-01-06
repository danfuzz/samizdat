// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Lowest-layer plumbing and data structures
//

#ifndef _DAT_H_
#define _DAT_H_

#include <stddef.h>

#include "module.h"
#include "ztype.h"

#include "dat/type.h"   // Types (must be included before other `dat` stuff).
#include "dat/frame.h"  // Frame (stack reference) management.
#include "dat/call.h"   // Function / method calling.


//
// Structures and constants
//

enum {
    /** Maximum number of symbols allowed. */
    DAT_MAX_SYMBOLS = 6000
};

/**
 * Partial definition of `DatHeader`, so that `classOf` and `datPayload`
 * can be defined as inlines.
 *
 * * **Note:** This must match the definition of `DatHeader` in `dat/impl.h`.
 */
typedef struct {
    zvalue private1;
    zvalue private2;
    zvalue cls;
    int private4 : 1;
    void *payload[/*flexible*/];
} DatHeaderExposed;


//
// Assertion Declarations
//

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


//
// Code Loading Declarations
//

/**
 * Loads and evaluates (runs) a native binary module, passing the given
 * `env` argument to its `eval` function. `path` indicates the filesystem
 * path to the module. This function returns whatever was returned by the
 * `eval` function.
 */
zvalue datEvalBinary(zvalue env, zvalue path);


//
// Memory Management Declarations
//

/**
 * Allocates a value, assigning it the given class, and sizing the memory
 * to include the given amount of extra bytes as raw payload data.
 * The resulting value is added to the live reference stack.
 */
zvalue datAllocValue(zvalue cls, zint extraBytes);

/**
 * Forces a gc.
 */
void datGc(void);

/**
 * Marks the given value as "immortal." It is considered a root and
 * will never get freed. Returns `value`, to aid in cascading calls (avoiding
 * duplication).
 */
zvalue datImmortalize(zvalue value);

/**
 * Marks a value during garbage collection. This in turn calls a class-specific
 * mark function when appropriate and may recurse arbitrarily. It is valid
 * to pass `NULL` to this, but no other non-values are acceptable.
 */
void datMark(zvalue value);

/**
 * Issues a fatal error about a void where a value was expected. This is used
 * by `datNonVoid()`.
 */
void datNonVoidError(void)
    __attribute__((noreturn));

/**
 * Checks that the given argument is non-void (that is not `NULL`), returning
 * it unmodified if non-void, or terminating the runtime with an error if it
 * is void.
 *
 * **Note:** This is not an `assert`, since it's meant to be used in a
 * full "production" type build.
 */
inline zvalue datNonVoid(zvalue value) {
    if (value == NULL) {
        datNonVoidError();
    }

    return value;
}

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
inline void *datPayload(zvalue value) {
    return ((DatHeaderExposed *) (void *) value)->payload;
}

/**
 * Gets the class of the given value. `value` must be a valid value (in
 * particular, non-`NULL`). The return value is of class `Class`.
 */
inline zvalue classOf(zvalue value) {
    return ((DatHeaderExposed *) (void *) value)->cls;
}

#endif
